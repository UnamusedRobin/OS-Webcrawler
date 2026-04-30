// C++ Program to Create a new thread using function pointer
// Function

#include <iostream>
#include <thread>

using namespace std;

// Function to be executed by the new thread
void myFunction()
{
    cout << "Hello from the new thread!" << endl;
}

int main()
{
    // Create a new thread that calls myFunction
    thread newThread(myFunction);

    // Wait for the new thread to finish execution
    //newThread.join();

    cout << "Hello from the main thread!" << endl;

    return 0;
}