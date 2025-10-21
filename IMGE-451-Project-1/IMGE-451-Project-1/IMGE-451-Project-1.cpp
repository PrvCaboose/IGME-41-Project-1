// IMGE-451-Project-1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <cctype>
#include <vector>
#include <memory>

using namespace std;


struct Process {
    int pid;
    int bytes;
    int numPages;
    vector<int> pageToFrame;
    vector<int> pageToBacking;
    //stats
    int cacheHits = 0, cacheMisses = 0;
    int memHits = 0, memMisses = 0;
    int backReads = 0, backWrites = 0;
};

struct Frame {
    bool isOccupied = false;
    int pid = -1;  // proccess occuping the frame
    int page = -1; // corresponding page to the process
    vector<int> data;
    bool isDirty = false;
    // replacement policy data
    int lastAccess = 0;
    int accessCount = 0;
};

struct CacheBlock {
    int valid = false;
    int pid = -1; // current process
    int page = -1; // corresponding page to the process
    vector<int> data;
    bool isDirty = false;
    int insertTime = 0; // for FIFO
};

struct BackingStore {
    int numBlocks = 0;
    int blockSize = 0;
    vector<vector<int>> blocks;
    vector<bool> allocatedBlocks;

};

// global vars
BackingStore backingStore;
vector<shared_ptr<Process>> processes;
shared_ptr<Process> currentProcess = nullptr;
int blockSize = 0; // global block size
int numPages = 0; // page offset i.e. p 4, 16 pages per PROCESS
int numCacheBlocks = 0;
int numFrames = 0;
int numBackBlocks = 0;
std::string mc = "t";
std::string mr_bs = "t";
std::string mr_rp = "lf";

bool CreateProcess(int pid, int bytes) {
    // Check if process exists
    for (int i = 0; i < processes.size(); i++)
    {
        if (processes[i]->pid == pid) {
            cout << "Error: pid already exists" << endl;
            return false;
        }
    }

    int numProcPages = (bytes + blockSize - 1) / blockSize;
    // Check if there is enough pages for the process
    if (numProcPages > numPages) {
        cout << "Error: process to big, too many pages required" << endl;
        return false;
    }

    // Check if there are enough free blocks in backing store
    int numFreeBlocks = 0;
    for (int i = 0; i < backingStore.allocatedBlocks.size(); i++)
    {
        if (!backingStore.allocatedBlocks[i]) {
            numFreeBlocks++;
        }
    }

    if (numProcPages > numFreeBlocks) {
        cout << "Error: Not enough memory in backing store, free up memory" << endl;
        return false;
    }

    // Create process
    shared_ptr<Process> proc = make_shared<Process>();
    proc->pid = pid;
    proc->bytes = bytes;
    proc->numPages = numProcPages;
    proc->pageToBacking.assign(numPages, -1);
    proc->pageToFrame.assign(numPages, -1);

    for (int i = 0; i < numProcPages; i++)
    {
        int backBlockIndex = -1;
        for (int i = 0; i < backingStore.numBlocks; i++)
        {
            if (!backingStore.allocatedBlocks[i]) {
                backingStore.allocatedBlocks[i] = true;
                backBlockIndex = i;
                break;
            }
        }

        proc->pageToBacking[i] = backBlockIndex;
    }
    
    return true;
}

int main(int argc, char* argv[])
{
    //sort through pre execution options
    
    blockSize = 0; // global block size
    numPages = 0; // page offset i.e. p 4, 16 pages per PROCESS
    numCacheBlocks = 0;
    numFrames = 0;
    numBackBlocks = 0;
    mc = "t";
    mr_bs = "t";
    mr_rp = "lf";
    bool simStart = false;

    std::string cmd = "";
    while (!simStart) {
        cout << "Enter command (B|P|C|F|S|MC|MR) or press 'z' to start sim: ";
        cin >> cmd;

        if (cmd == "b" || cmd == "B") {
            cout << "B <bits>, Enter Bit width for OFFSET: ";
            cin >> blockSize;
        }
        else if (cmd == "p" || cmd == "P") {
            cout << "P <bits>, Enter Bit width for PAGE: ";
            cin >> numPages;
        }
        else if (cmd == "c" || cmd == "C") {
            cout << "C <bits>, Enter Bit width for CACHE BLOCK: ";
            cin >> numCacheBlocks;
        }
        else if (cmd == "f" || cmd == "F") {
            cout << "F <bits>, Enter Bit width for FRAME: ";
            cin >> numFrames;
        }
        else if (cmd == "s" || cmd == "S") {
            cout << "S <bits>, Enter Bit width for BACKING STORE block: ";
            cin >> numBackBlocks;
        }
        else if (cmd == "mc" || cmd == "MC") {
            cout << "MC <B|T>, Chache mode, write (b)ack or write (t)hrough: ";
            cin >> mc;
            // turn strings to lower case
            transform(mc.begin(), mc.end(), mc.begin(), [](unsigned char c) {return tolower(c); });
        }
        else if (cmd == "mr" || cmd == "MR") {
                cout << "MR <B|T> <LR|MR|MF|LF>, ";
                cin >> mr_bs >> mr_rp;
                // turn strings to lower case
                transform(mr_bs.begin(), mr_bs.end(), mr_bs.begin(), [](unsigned char c) {return tolower(c); });
                transform(mr_rp.begin(), mr_rp.end(), mr_rp.begin(), [](unsigned char c) {return tolower(c); });
        }
        else if (cmd == "Z" || cmd == "z") {
            simStart = true;
            if (blockSize <= 0 || numPages <= 0 || numCacheBlocks <= 0 || numFrames <= 0 || numBackBlocks <= 0) {
                cout << "Not all values have been set" << endl;
                simStart = false;
            }
            if ((mc != "b" && mc != "t") || (mr_bs != "b" && mr_bs != "t") || (mr_rp != "lr" && mr_rp != "mr" &&mr_rp != "mf" &&mr_rp != "lf")) {
                cout << "Invalid cache or ram modes selected" << endl;
                simStart = false;
            }
        }
    }
    cout << "Sim started!" << endl;
    
    // Setup data
    blockSize = 1 < blockSize;
    numPages = 1 < numPages;
    numCacheBlocks = 1 < numCacheBlocks;
    numFrames = 1 < numFrames;
    numBackBlocks = 1 < numBackBlocks;


    backingStore.numBlocks = numBackBlocks;
    backingStore.blockSize = blockSize;
    backingStore.blocks.assign(numBackBlocks, vector<int>(blockSize, 0)); // Init blocks 2D vector [numBlocks][blockSize]
    backingStore.allocatedBlocks.assign(numBackBlocks, false); // Init allocated vector to false

    
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
