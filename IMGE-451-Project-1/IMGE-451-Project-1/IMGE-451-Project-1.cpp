// IMGE-451-Project-1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <math.h>
#include <algorithm>
#include <cctype>
#include <vector>

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
    int numBlocks;
    int blockSize;
    vector<vector<int>> blocks;
    vector<bool> allocatedBlocks;

};

int main(int argc, char* argv[])
{
    //sort through pre execution options
    
    int blockSize = 0; // global block size
    int numPages = 0; // page offset i.e. p 4, 16 pages per PROCESS
    int numCacheBlocks = 0;
    int numFrames = 0;
    int numBackBlocks = 0;
    std::string mc = "t";
    std::string mr_bs = "t";
    std::string mr_rp = "lf";
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

    BackingStore backingStore;
    backingStore.numBlocks = numBackBlocks;
    backingStore.blockSize = blockSize;
    backingStore.blocks.assign(numBackBlocks, vector<int>(blockSize, 0));
    
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
