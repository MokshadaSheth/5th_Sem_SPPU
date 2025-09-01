#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
using namespace std;

// Function to display current frame contents
void displayFrames(const vector<int>& frames) {
    cout << "[ ";
    for (int f : frames) cout << f << " ";
    cout << "]";
}

// FIFO Algorithm
void fifo(vector<int> pages, int capacity) {
    cout << "\n===== FIFO Page Replacement =====\n";
    vector<int> frames;
    int misses = 0;

    for (int i = 0; i < pages.size(); i++) {
        int page = pages[i];
        cout << "Step " << i+1 << " | Page: " << page << " | ";

        if (find(frames.begin(), frames.end(), page) == frames.end()) {
            if (frames.size() < capacity)
                frames.push_back(page);
            else {
                frames.erase(frames.begin());
                frames.push_back(page);
            }
            misses++;
            cout << "Miss ";
        } else {
            cout << "Hit  ";
        }
        displayFrames(frames);
        cout << endl;
    }
    cout << "Total Misses (FIFO): " << misses << "\n";
}

// LRU Algorithm
void lru(vector<int> pages, int capacity) {
    cout << "\n===== LRU Page Replacement =====\n";
    vector<int> frames;
    unordered_map<int, int> lastUsed;
    int misses = 0;

    for (int i = 0; i < pages.size(); i++) {
        int page = pages[i];
        cout << "Step " << i+1 << " | Page: " << page << " | ";

        if (find(frames.begin(), frames.end(), page) == frames.end()) {
            if (frames.size() < capacity) {
                frames.push_back(page);
            } else {
                // find least recently used
                int lruPage = frames[0], minIndex = lastUsed[lruPage];
                for (int f : frames) {
                    if (lastUsed[f] < minIndex) {
                        minIndex = lastUsed[f];
                        lruPage = f;
                    }
                }
                replace(frames.begin(), frames.end(), lruPage, page);
            }
            misses++;
            cout << "Miss ";
        } else {
            cout << "Hit  ";
        }
        lastUsed[page] = i;
        displayFrames(frames);
        cout << endl;
    }
    cout << "Total Misses (LRU): " << misses << "\n";
}

// Optimal Algorithm
void optimal(vector<int> pages, int capacity) {
    cout << "\n===== Optimal Page Replacement =====\n";
    vector<int> frames;
    int misses = 0;

    for (int i = 0; i < pages.size(); i++) {
        int page = pages[i];
        cout << "Step " << i+1 << " | Page: " << page << " | ";

        if (find(frames.begin(), frames.end(), page) == frames.end()) {
            if (frames.size() < capacity) {
                frames.push_back(page);
            } else {
                // predict future use
                int farthest = i+1, victim = frames[0];
                for (int f : frames) {
                    auto it = find(pages.begin()+i+1, pages.end(), f);
                    if (it == pages.end()) {
                        victim = f;
                        break;
                    } else if (it - pages.begin() > farthest) {
                        farthest = it - pages.begin();
                        victim = f;
                    }
                }
                replace(frames.begin(), frames.end(), victim, page);
            }
            misses++;
            cout << "Miss ";
        } else {
            cout << "Hit  ";
        }
        displayFrames(frames);
        cout << endl;
    }
    cout << "Total Misses (Optimal): " << misses << "\n";
}

int main() {
    int n, capacity, choice;
    cout << "Enter number of pages: ";
    cin >> n;

    vector<int> pages(n);
    cout << "Enter the page reference string: ";
    for (int i = 0; i < n; i++) cin >> pages[i];

    cout << "Enter frame capacity: ";
    cin >> capacity;

    do {
        cout << "\n===== Page Replacement Menu =====\n";
        cout << "1. FIFO\n";
        cout << "2. LRU\n";
        cout << "3. Optimal\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1: fifo(pages, capacity); break;
            case 2: lru(pages, capacity); break;
            case 3: optimal(pages, capacity); break;
            case 4: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice! Try again.\n";
        }
    } while(choice != 4);

    return 0;
}