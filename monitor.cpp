#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <limits>  // For numeric_limits
#include <fstream> // For data logging
using namespace std;

// Color definitions
#define RESET "\033[0m"
#define RED "\033[1;91m"
#define GREEN "\033[1;92m"
#define YELLOW "\033[1;93m"
#define CYAN "\033[1;96m"
#define WHITE "\033[1;97m"
#define BOLD "\033[1m"

// Clear screen and add margins
void clearScreen()
{
    system("clear || cls");
    cout << "\n\n";
}

// Function to display messages with left margin
void printWithMargin(const string &message, const string &color = WHITE)
{
    cout << "\t\t" << color << message << RESET;
}

// Base class for devices
class Device
{
protected:
    string name;
    bool isOn;

public:
    Device(string deviceName) : name(deviceName), isOn(false) {}

    virtual void toggle()
    {
        isOn = !isOn;
        printWithMargin(name + (isOn ? " turned ON.\n" : " turned OFF.\n"), isOn ? GREEN : RED);
    }

    virtual void showState() const
    {
        printWithMargin(name + " is " + (isOn ? "ON\n" : "OFF\n"), isOn ? GREEN : RED);
    }

    virtual void adjustSetting(int value)
    {
        printWithMargin("No adjustable settings for this device.\n", YELLOW);
    }

    string getName() const
    {
        return name;
    }

    bool getState() const
    {
        return isOn;
    }
};

// Derived class for fans
class Fan : public Device
{
    string speed; // Full, Medium, Low, Off

public:
    Fan(string deviceName) : Device(deviceName), speed("Off") {}

    void toggle() override
    {
        isOn = !isOn;
        if (isOn)
        {
            speed = "Medium"; // Automatically set speed to Medium when toggled on
            printWithMargin(name + " turned ON with speed set to Medium.\n", GREEN);
        }
        else
        {
            speed = "Off"; // Set speed to Off when toggled off
            printWithMargin(name + " turned OFF.\n", RED);
        }
    }

    void adjustSetting(int value) override
    {
        if (value < 0 || value > 3)
        {
            printWithMargin("Invalid speed! Must be 0 (Off), 1 (Low), 2 (Medium), or 3 (Full).\n", RED);
        }
        else
        {
            if (value == 0)
            {
                speed = "Off";
                isOn = false;
            }
            else
            {
                if (!isOn)
                {
                    isOn = true; // Automatically turn the fan on if adjusting speed
                    printWithMargin(name + " turned ON.\n", GREEN);
                }
                // Set the speed as per the user's input
                switch (value)
                {
                case 1:
                    speed = "Low";
                    break;
                case 2:
                    speed = "Medium";
                    break;
                case 3:
                    speed = "Full";
                    break;
                }
            }
            printWithMargin(name + " speed set to " + speed + ".\n", CYAN);
        }
    }

    void showState() const override
    {
        printWithMargin(name + " is " + (isOn ? "ON" : "OFF") + " with speed " + speed + ".\n", isOn ? GREEN : RED);
    }
};

// Derived class for cameras
class Camera : public Device
{
public:
    Camera(string deviceName) : Device(deviceName) {}

    void toggle() override
    {
        isOn = !isOn;
        printWithMargin(name + (isOn ? " started recording.\n" : " is now idle.\n"), isOn ? GREEN : RED);
    }

    void showState() const override
    {
        printWithMargin(name + (isOn ? " is recording.\n" : " is idle.\n"), isOn ? GREEN : RED);
    }
};

// Derived class for lights
class Light : public Device
{
    int brightness; // 0 to 100

public:
    Light(string deviceName) : Device(deviceName), brightness(50) {}

    void toggle() override
    {
        isOn = !isOn;
        if (isOn)
        {
            brightness = 65; // Set default brightness to 65% when toggled on
            printWithMargin(name + " turned ON with brightness set to 65%.\n", GREEN);
        }
        else
        {
            printWithMargin(name + " turned OFF.\n", RED);
        }
    }

    void adjustSetting(int value) override
    {
        if (value < 0 || value > 100)
        {
            printWithMargin("Invalid brightness value! Must be between 0 and 100.\n", RED);
        }
        else
        {
            if (!isOn)
            {
                isOn = true; // Automatically turn the light on
                printWithMargin(name + " turned ON.\n", GREEN);
            }
            brightness = value;
            printWithMargin(name + " brightness set to " + to_string(brightness) + "%.\n", CYAN);
        }
    }

    void showState() const override
    {
        printWithMargin(name + " is " + (isOn ? "ON" : "OFF") + " with brightness " + to_string(brightness) + "%.\n", isOn ? GREEN : RED);
    }
};

// Derived class for thermostat
class Thermostat : public Device
{
    int temperature; // 16 to 30

public:
    Thermostat(string deviceName) : Device(deviceName), temperature(22) {}

    void adjustSetting(int value) override
    {
        if (value < 16 || value > 30)
        {
            printWithMargin("Invalid temperature! Must be between 16 and 30 degrees.\n", RED);
        }
        else
        {
            temperature = value;
            printWithMargin(name + " temperature set to " + to_string(temperature) + "°C.\n", CYAN);
        }
    }

    void showState() const override
    {
        printWithMargin(name + " is " + (isOn ? "ON" : "OFF") + " with temperature " + to_string(temperature) + "°C.\n", isOn ? GREEN : RED);
    }
};

// Class for managing smart home
class SmartHome
{
    vector<Device *> devices;
    ofstream logFile;
public:
    SmartHome()
    {
        // Open log file
        logFile.open("smart_home_log.txt", ios::app);
        if (!logFile.is_open())
        {
            printWithMargin("Failed to open log file!\n", RED);
        }
    }
    ~SmartHome()
    {
        // Close log file
        if (logFile.is_open())
        {
            logFile.close();
        }

        // Delete allocated devices
        for (auto device : devices)
        {
            delete device;
        }
    }
    void addDevice(Device *device)
    {
        devices.push_back(device);
    }
    void listDevices(){
        printWithMargin("Available Devices:\n", BOLD);
        for (size_t i = 0; i < devices.size(); ++i)
        {
            cout << "\t\t" << i + 1 << ". " << devices[i]->getName() << endl;
        }
    }
    void controlDevice(){
        listDevices();
        printWithMargin("Enter the device number to control: ", CYAN);
        int choice = getIntInput();
        if (choice < 1 || choice > static_cast<int>(devices.size()))
        {
            printWithMargin("Invalid device number!\n", RED);
            return;
        }
        Device *device = devices[choice - 1];
        clearScreen();
        printWithMargin("1. Toggle ON/OFF\n", BOLD);
        printWithMargin("2. Adjust settings\n", BOLD);
        printWithMargin("3. Show state\n", BOLD);
        printWithMargin("Enter your choice: ", CYAN);
        int action = getIntInput();
        clearScreen();
        switch (action)
        {
        case 1:
            device->toggle();
            logAction(device->getName(), "Toggled " + string(device->getState() ? "ON" : "OFF"));
            break;
        case 2:
        {
            printWithMargin("\nEnter the value to adjust (e.g., brightness/speed/temperature): ", CYAN);
            int value = getIntInput();
            device->adjustSetting(value);
            logAction(device->getName(), "Adjusted setting to " + to_string(value));
            break;
        }
        case 3:
            device->showState();
            break;
        default:
            printWithMargin("Invalid choice!\n", RED);
        }
    }
    void clearLog(){
        ifstream logFileRead("smart_home_log.txt");
        if (logFileRead.peek() == ifstream::traits_type::eof())
        {
            // Check if the file is empty
            printWithMargin("No logs found. Log file is already empty.\n", YELLOW);
        }
        else
        {
            ofstream logFileWrite("smart_home_log.txt", ios::trunc); // Clear the file
            if (logFileWrite.is_open())
            {
                logFileWrite.close();
                printWithMargin("Log file cleared successfully.\n", GREEN);
            }
            else
            {
                printWithMargin("Failed to clear log file.\n", RED);
            }
        }
        logFileRead.close();
    }
    void resetDevices(){
        for (auto device : devices)
        {
            if (device->getState())
            {
                device->toggle(); // Turn off all devices
            }
        }
        printWithMargin("All devices have been reset to their OFF state.\n", GREEN);
        logAction("System", "All devices reset to OFF");
    }
    void logAction(const string &deviceName, const string &action){
        if (logFile.is_open())
        {
            logFile << deviceName << ": " << action << endl;
        }
    }
    void viewLog(){
        clearScreen();
        ifstream logInput("smart_home_log.txt");
        if (!logInput.is_open())
        {
            printWithMargin("No logs available!\n", RED);
            return;
        }
        string line;
        printWithMargin("Action Logs:\n", BOLD);
        while (getline(logInput, line))
        {
            cout << "\t\t" << line << endl;
        }
        logInput.close();
    }
    int getIntInput(){
        int value;
        cout << flush; // Ensure prompt is shown before input
        while (!(cin >> value))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            printWithMargin("Invalid input! Please enter a number: ", RED);
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear newline character
        return value;
    }
    void exitSystem(){
        clearScreen();
        printWithMargin("Are you sure you want to exit? (y/n): ", YELLOW);
        char choice;
        cin >> choice;
        if (choice == 'y' || choice == 'Y')
        {
            printWithMargin("Exiting system. Goodbye!\n\a", GREEN);
            exit(0);
        }
    }
};

int main()
{
    SmartHome home;

    // Adding devices
    home.addDevice(new Light("Living Room Light"));
    home.addDevice(new Light("Bedroom Light"));
    home.addDevice(new Fan("Living Room Fan"));
    home.addDevice(new Fan("Bedroom Fan"));
    home.addDevice(new Camera("Front Door Camera"));
    home.addDevice(new Camera("Backyard Camera"));
    home.addDevice(new Thermostat("Home Thermostat"));

    while (true)
    {
        clearScreen();
        printWithMargin("\n\tSMART HOME AUTOMATION SYSTEM\n\n", BOLD);
        printWithMargin("1. List devices\n", BOLD);
        printWithMargin("2. Control a device\n", BOLD);
        printWithMargin("3. View log\n", BOLD);
        printWithMargin("4. Clear log\n", BOLD);
        printWithMargin("5. Reset all devices\n", BOLD);

        printWithMargin("6. Exit\n", BOLD);
        printWithMargin("\n\tEnter your choice: ", CYAN);

        int choice = home.getIntInput();
        switch (choice)
        {
        case 1:
            home.listDevices();
            break;
        case 2:
            home.controlDevice();
            break;
        case 3:
            home.viewLog();
            break;
        case 4:
            home.clearLog();
            break;
        case 5:
            home.resetDevices();
            break;
        case 6:
            home.exitSystem();
            break;
        default:
            printWithMargin("\tInvalid choice! Try again.\n", RED);
        }

        printWithMargin("\tPress Enter to continue...", YELLOW);
        cin.ignore();
    }

    return 0;
}
