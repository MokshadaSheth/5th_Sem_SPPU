#include <iostream>
#include <mutex>
#include <thread>
#include <semaphore>
#include <vector>
#include <atomic>
#include <iomanip>  // for setw()
#include <chrono>
#include <algorithm>

using namespace std;

class Mutex_app
{
    int balance;
    string name;
    mutex mtx;
    mutex print_mtx;
    atomic<int> current_withdrawing_user{-1};
    vector<int> waiting_queue;
    mutex queue_mtx;

public:
    Mutex_app() : balance(-1), name("") {}
    Mutex_app(int b, string n) : balance(b), name(n) {}

    void withdraw(int user_id)
    {
        {
            lock_guard<mutex> lock(queue_mtx);
            waiting_queue.push_back(user_id);
        }

        printStatus();

        mtx.lock();

        {
            lock_guard<mutex> lock(queue_mtx);
            // Remove from waiting queue once allowed to withdraw
            waiting_queue.erase(remove(waiting_queue.begin(), waiting_queue.end(), user_id), waiting_queue.end());
        }

        current_withdrawing_user = user_id;
        printStatus();

        cout << "\n[Mutex] User " << user_id << " started withdrawing.\n";

        // for (int i = 0; i < 2; i++)
        // {
            int amt;
            cout << "[User " << user_id << "] Enter amount to withdraw: ";
            cin >> amt;

            if (amt > balance)
            {
                cout << "[User " << user_id << "] Withdrawal Not Possible. Insufficient Balance.\n";
            }
            else
            {
                balance -= amt;
                cout << "[User " << user_id << "] Withdrawn " << amt << ". New Balance: " << balance << endl;
            }
        // }

        cout << "[Mutex] User " << user_id << " finished withdrawing.\n";

        current_withdrawing_user = -1;

        printStatus();

        mtx.unlock();
    }

    void printStatus()
    {
        lock_guard<mutex> lock(print_mtx);
        cout << "\n==================== WITHDRAWAL STATUS ====================\n";
        if (current_withdrawing_user != -1)
            cout << "Currently Withdrawing: User " << current_withdrawing_user.load() << "\n";
        else
            cout << "Currently Withdrawing: None\n";

        cout << "Waiting Queue: ";
        if (waiting_queue.empty())
            cout << "None";
        else
        {
            for (auto id : waiting_queue)
                cout << "User " << id << "  ";
        }
        cout << "\n============================================================\n";
    }
};


class Semaphore_app
{
    std::counting_semaphore<3> resource_semaphore{3};
    mutex print_mtx;
    atomic<int> current_in_kitchen{0};
    vector<int> waiting_queue;
    mutex queue_mtx;

public:
    void useKitchen(int user_id)
    {
        {
            lock_guard<mutex> lock(queue_mtx);
            waiting_queue.push_back(user_id);
        }

        printStatus();

        resource_semaphore.acquire();

        {
            lock_guard<mutex> lock(queue_mtx);
            // Remove from waiting queue once allowed in
            waiting_queue.erase(remove(waiting_queue.begin(), waiting_queue.end(), user_id), waiting_queue.end());
        }

        current_in_kitchen++;

        printStatus();

        {
            lock_guard<mutex> lock(print_mtx);
            cout << "[Semaphore] User " << user_id << " (Thread " << this_thread::get_id() << ") entered the kitchen.\n";
        }

        this_thread::sleep_for(chrono::seconds(2));

        {
            lock_guard<mutex> lock(print_mtx);
            cout << "[Semaphore] User " << user_id << " (Thread " << this_thread::get_id() << ") leaving the kitchen.\n";
        }

        current_in_kitchen--;

        resource_semaphore.release();

        printStatus();
    }

    void printStatus()
    {
        lock_guard<mutex> lock(print_mtx);
        cout << "\n==================== KITCHEN STATUS ====================\n";
        cout << "Total Slots Available: " << (3 - current_in_kitchen.load()) << "/3\n";
        cout << "Currently Cooking: " << current_in_kitchen.load() << "\n";
        cout << "Waiting Queue: ";
        if (waiting_queue.empty())
            cout << "None";
        else
        {
            for (auto id : waiting_queue)
                cout << "User " << id << "  ";
        }
        cout << "\n=========================================================\n";
    }
};


int main()
{
    int choice = 0;
    Mutex_app mutObj(50000, "ABC");
    Semaphore_app semObj;

    while (choice != 3)
    {
        cout << "\nMenu:\n1. Mutex Withdrawal\n2. Semaphore Kitchen\n3. Exit\nEnter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            cout << "\n[Mutex Application] Starting...\n";
            thread t1(&Mutex_app::withdraw, &mutObj, 1);
            thread t2(&Mutex_app::withdraw, &mutObj, 2);
            thread t3(&Mutex_app::withdraw, &mutObj, 3);
            thread t4(&Mutex_app::withdraw, &mutObj, 4);
            thread t5(&Mutex_app::withdraw, &mutObj, 5);


            t1.join();
            t2.join();
            t3.join();
            t4.join();
            t5.join();
            cout << "[Mutex Application] Finished.\n";
        }
        else if (choice == 2)
        {
            cout << "\n[Semaphore Application] Starting...\n";
            vector<thread> threads;
            int num_users = 5;

            for (int i = 1; i <= num_users; ++i)
            {
                threads.emplace_back(&Semaphore_app::useKitchen, &semObj, i);
            }

            for (auto &t : threads)
                t.join();

            cout << "[Semaphore Application] Finished.\n";
        }
        else if (choice == 3)
        {
            cout << "Exiting program...\n";
        }
        else
        {
            cout << "Invalid choice. Try again.\n";
        }
    }

    return 0;
}
