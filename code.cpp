#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <stdexcept>  // For exception handling
using namespace std;

void printSeparator();
void printHeader();
void displayLoginPage();
void displayCarParkingMenu();
void printExitMessage();

class Login {
public:
    virtual bool authenticate() = 0;
};

class AdminLogin : public Login {
private:
    string password = "1234";
public:
    bool authenticate() override {
        string passwordAttempt;
        char ch;

        displayLoginPage();
        cout << "\n\t\t\t\t\t    Enter Password: ";
        ch = _getch();
        while (ch != 13) {
            passwordAttempt.push_back(ch);
            cout << '*';
            ch = _getch();
        }

        if (passwordAttempt == password) {
            cout << "\n\n\t\t\t\t\t\t    Access Granted!";
            printSeparator();
            return true;
        } else {
            cout << "\n\n\t\t\t\t\t\t    Access Denied. Try Again!";
            printSeparator();
            return false;
        }
    }
};

class Vehicle {
protected:
    string driverName;
    string registrationNumber;
    string companyAndModel;
    int timeStay;
    double parkingFee;

public:
    Vehicle() : driverName(""), registrationNumber(""), companyAndModel(""), timeStay(0), parkingFee(0) {}

    void setVehicleDetails(string driver, string regNo, string companyModel, int timeStay, double fee) {
        this->driverName = driver;
        this->registrationNumber = regNo;
        this->companyAndModel = companyModel;
        this->timeStay = timeStay;
        this->parkingFee = fee;
    }

    void displayVehicleDetails() const {
        cout << "\n\t\t\t\t\t    ---- Vehicle Details ----";
        printSeparator();
        cout << "\n\t\t\t\tCompany and Model: " << companyAndModel
             << "\n\t\t\t\tRegistration Number: " << registrationNumber
             << "\n\t\t\t\tDriver Name: " << driverName
             << "\n\t\t\t\tTime Stay: " << timeStay << " hours"
             << "\n\t\t\t\tParking Fee: Rs." << parkingFee;
        printSeparator();
    }

    string getRegistrationNumber() const {
        return registrationNumber;
    }

    double getParkingFee() const {
        return parkingFee;
    }

    void clearDetails() {
        driverName = "";
        registrationNumber = "";
        companyAndModel = "";
        timeStay = 0;
        parkingFee = 0;
    }

    string toFileString() const {
        return driverName + "," + registrationNumber + "," + companyAndModel + "," + to_string(timeStay) + "," + to_string(parkingFee) + "\n";
    }

    static Vehicle fromFileString(const string& line) {
        Vehicle v;
        size_t pos = 0;
        string token;
        string lineCopy = line;
        int fieldIndex = 0;

        while ((pos = lineCopy.find(",")) != string::npos) {
            token = lineCopy.substr(0, pos);
            switch (fieldIndex) {
                case 0: v.driverName = token; break;
                case 1: v.registrationNumber = token; break;
                case 2: v.companyAndModel = token; break;
                case 3: v.timeStay = stoi(token); break;
                case 4: v.parkingFee = stod(token); break;
            }
            lineCopy.erase(0, pos + 1);
            fieldIndex++;
        }

        return v;
    }
};

class ParkingManagement {
public:
    virtual void parkVehicle() = 0;
    virtual void viewVehicleDetails() = 0;
    virtual void removeVehicle() = 0;
};

class ParkingSystem : public ParkingManagement {
private:
    Vehicle cars[10];
    Vehicle bikes[10];
    int carCount;
    int bikeCount;

public:
    ParkingSystem() : carCount(0), bikeCount(0) {
        loadVehiclesFromFile();
    }

    void loadVehiclesFromFile() {
        try {
            ifstream infile("vehicles.txt");
            if (!infile) throw runtime_error("Error opening file!");

            string line;
            while (getline(infile, line)) {
                Vehicle v = Vehicle::fromFileString(line);
                if (line.find("Car") != string::npos) {
                    cars[carCount++] = v;
                } else if (line.find("Bike") != string::npos) {
                    bikes[bikeCount++] = v;
                }
            }
            infile.close();
        } catch (const runtime_error& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }

    void saveVehiclesToFile() {
        try {
            ofstream outfile("vehicles.txt");
            if (!outfile) throw runtime_error("Error opening file!");

            for (int i = 0; i < carCount; i++) {
                outfile << cars[i].toFileString();
            }
            for (int i = 0; i < bikeCount; i++) {
                outfile << bikes[i].toFileString();
            }
            outfile.close();
        } catch (const runtime_error& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }

    void parkVehicle() override {
        try {
            int choice;
            cout << "\n\t\t\t\tChoose Vehicle Type:\n";
            cout << "\t\t\t\t1. Car (Rate: Rs. 100/hour)\n";
            cout << "\t\t\t\t2. Bike (Rate: Rs. 50/hour)\n";
            cin >> choice;

            string driver, regNo, companyModel;
            int timeStay;
            double rate = (choice == 1) ? 100.0 : 50.0;

            cout << "\n\t\t\t\tEnter Driver Name: ";
            cin.ignore();
            getline(cin, driver);
            cout << "\t\t\t\tEnter Registration Number: ";
            getline(cin, regNo);
            cout << "\t\t\t\tEnter Company and Model: ";
            getline(cin, companyModel);
            cout << "\t\t\t\tEnter Time Stay (in hours): ";
            cin >> timeStay;

            double totalFee = timeStay * rate;

            if (choice == 1 && carCount < 10) {
                cars[carCount].setVehicleDetails(driver, regNo, companyModel, timeStay, totalFee);
                carCount++;
            } else if (choice == 2 && bikeCount < 10) {
                bikes[bikeCount].setVehicleDetails(driver, regNo, companyModel, timeStay, totalFee);
                bikeCount++;
            } else {
                cout << "\n\t\t\t\tParking Lot Full or Invalid Vehicle Type!";
                return;
            }

            cout << "\n\t\t\t\tVehicle Parked Successfully! Total Fee: Rs." << totalFee;

            saveVehiclesToFile();  // Save updated vehicles to file
        } catch (const exception& e) {
            cout << "\nError: " << e.what() << endl;
        }
    }

    void viewVehicleDetails() override {
        string regNo;
        cout << "\n\t\t\t\tEnter Registration Number to view details: ";
        cin >> regNo;

        bool found = false;
        for (int i = 0; i < carCount; i++) {
            if (cars[i].getRegistrationNumber() == regNo) {
                cars[i].displayVehicleDetails();
                found = true;
                break;
            }
        }
        for (int i = 0; i < bikeCount && !found; i++) {
            if (bikes[i].getRegistrationNumber() == regNo) {
                bikes[i].displayVehicleDetails();
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "\n\t\t\t\tVehicle not found!";
        }
    }

    void removeVehicle() override {
        string regNo;
        cout << "\n\t\t\t\tEnter Registration Number to remove: ";
        cin >> regNo;

        bool found = false;
        for (int i = 0; i < carCount; i++) {
            if (cars[i].getRegistrationNumber() == regNo) {
                cars[i].clearDetails();
                carCount--;
                found = true;
                break;
            }
        }
        for (int i = 0; i < bikeCount && !found; i++) {
            if (bikes[i].getRegistrationNumber() == regNo) {
                bikes[i].clearDetails();
                bikeCount--;
                found = true;
                break;
            }
        }

        if (!found) {
            cout << "\n\t\t\t\tVehicle not found!";
        } else {
            cout << "\n\t\t\t\tVehicle Removed Successfully!";
        }

        saveVehiclesToFile();  // Save updated vehicles to file
    }
};

void printSeparator() {
    cout << "\n-----------------------------------------------------------\n";
}

void printHeader() {
    cout << "\n\n\n\n";
    cout << "\t\t\t\t###############################################################\n";
    cout << "\t\t\t\t#                                                             #\n";
    cout << "\t\t\t\t#                       OOP Project                           #\n";
    cout << "\t\t\t\t#                                                             #\n";
    cout << "\t\t\t\t###############################################################\n";
    cout << "\n\n\n";
    cout << "\t\t\t\t----------->  WELCOME TO THE VEHICLE PARKING SYSTEM  <----------\n";
    printSeparator();
}

void displayLoginPage() {
    printHeader();
    cout << "\n\t\t\t\tVehicle Parking System Login\n";
    printSeparator();
    cout << "\n\n\t\t\t\tPlease enter the password to proceed:\n";
}

void displayCarParkingMenu() {
    cout << "---------------------------------------------------" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "1. Park Your Car " << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "2. View Your Car Details" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "3. Check Expenses" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "4. Remove Your Car" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "5. EXIT" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "ENTER YOUR CHOICE : ";
}

void printExitMessage() {
    cout << "Good Bye" << endl
         << "Thanks For Choosing Us!" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "Press Enter Key To Return" << endl;
}

class VehicleParkingSystem {
private:
    AdminLogin login;
    ParkingSystem parking;

public:
    void run() {
        if (login.authenticate()) {
            bool running = true;
            while (running) {
                displayCarParkingMenu();
                int choice;
                cin >> choice;

                switch (choice) {
                    case 1:
                        parking.parkVehicle();
                        break;
                    case 2:
                        parking.viewVehicleDetails();
                        break;
                    case 3:
                        parking.removeVehicle();
                        break;
                    case 4:
                        parking.removeVehicle();
                        break;
                    case 5:
                        running = false;
                        break;
                    default:
                        cout << "\n\t\t\t\tInvalid choice! Please try again.\n";
                }
            }
        }
    }
};

int main() {
    VehicleParkingSystem system;
    system.run();
    return 0;
}  