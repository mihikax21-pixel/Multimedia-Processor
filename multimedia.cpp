#include <iostream>
#include <string>
#include <fstream>
#include <queue>
using namespace std;

// ===============================
// BASE CLASS
// ===============================

class Multimedia {
protected:
    string fileName;
    string fileType;

public:
    Multimedia(string name, string type) {
        fileName = name;
        fileType = type;
    }

    bool checkFile() {
        ifstream file(fileName, ios::binary);

        if (file.good()) {
            cout << "\nFile found successfully!\n";
            return true;
        }
        else {
            cout << "\nFile not found!\n";
            return false;
        }
    }

    virtual void process() = 0;

    virtual ~Multimedia() {}
};


// ===============================
// IMAGE PROCESSING CLASS
// ===============================

class Image : public Multimedia {

public:
    Image(string name) : Multimedia(name, "Image") {}

    void process() override {

        if (!checkFile())
            return;

        int choice;

        cout << "\n--- IMAGE PROCESSING ---\n";
        cout << "1. Resize Image\n";
        cout << "2. Convert to Grayscale\n";
        cout << "3. Apply Filter\n";
        cout << "4. Back\n";

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            cout << "\nEnter new width: ";
            int width;
            cin >> width;

            cout << "Enter new height: ";
            int height;
            cin >> height;

            cout << "\nImage resize operation selected.";
            cout << "\nNew dimensions: "
                 << width << " x " << height << endl;
            break;

        case 2:
            cout << "\nGrayscale conversion selected.\n";
            break;

        case 3:
            cout << "\nAvailable Filters:\n";
            cout << "1. Blur\n";
            cout << "2. Sharpen\n";
            cout << "3. Brightness\n";
            cout << "Select a filter: ";

            int filter;
            cin >> filter;

            cout << "\nFilter operation selected.\n";
            break;

        case 4:
            return;

        default:
            cout << "\nInvalid choice!\n";
        }
    }
};


// ===============================
// AUDIO PROCESSING CLASS
// ===============================

class Audio : public Multimedia {

public:
    Audio(string name) : Multimedia(name, "Audio") {}

    void process() override {

        if (!checkFile())
            return;

        int choice;

        cout << "\n--- AUDIO PROCESSING ---\n";
        cout << "1. Adjust Volume\n";
        cout << "2. Trim Audio\n";
        cout << "3. Noise Reduction\n";
        cout << "4. Back\n";

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            cout << "\nEnter volume level (0-200): ";

            int volume;
            cin >> volume;

            if (volume >= 0 && volume <= 200)
                cout << "\nVolume adjustment selected: "
                     << volume << "%\n";
            else
                cout << "\nInvalid volume level!\n";

            break;

        case 2:
            {
                float start, end;

                cout << "\nEnter start time (seconds): ";
                cin >> start;

                cout << "Enter end time (seconds): ";
                cin >> end;

                if (start >= 0 && end > start)
                    cout << "\nAudio trimming selected from "
                         << start << " to " << end << " seconds.\n";
                else
                    cout << "\nInvalid time range!\n";

                break;
            }

        case 3:
            cout << "\nNoise reduction operation selected.\n";
            break;

        case 4:
            return;

        default:
            cout << "\nInvalid choice!\n";
        }
    }
};


// ===============================
// VIDEO PROCESSING CLASS
// ===============================

class Video : public Multimedia {

public:
    Video(string name) : Multimedia(name, "Video") {}

    void process() override {

        if (!checkFile())
            return;

        int choice;

        cout << "\n--- VIDEO PROCESSING ---\n";
        cout << "1. Trim Video\n";
        cout << "2. Change Resolution\n";
        cout << "3. Extract Frames\n";
        cout << "4. Video Compression\n";
        cout << "5. Back\n";

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {

        case 1:
            {
                float start, end;

                cout << "\nEnter starting time: ";
                cin >> start;

                cout << "Enter ending time: ";
                cin >> end;

                if (start >= 0 && end > start)
                    cout << "\nVideo trimming selected from "
                         << start << " to " << end << " seconds.\n";
                else
                    cout << "\nInvalid time range!\n";

                break;
            }

        case 2:
            {
                int width, height;

                cout << "\nEnter new width: ";
                cin >> width;

                cout << "Enter new height: ";
                cin >> height;

                cout << "\nResolution change selected: "
                     << width << " x " << height << endl;

                break;
            }

        case 3:
            cout << "\nFrame extraction operation selected.\n";
            break;

        case 4:
            cout << "\nVideo compression operation selected.\n";
            break;

        case 5:
            return;

        default:
            cout << "\nInvalid choice!\n";
        }
    }
};


// ===============================
// MAIN FUNCTION
// ===============================

int main() {

    int choice;
    string fileName;

    // DSA: Queue for processing requests
    queue<string> processingQueue;

    cout << "=================================\n";
    cout << "      MULTIMEDIA PROCESSOR\n";
    cout << "=================================\n";

    do {

        cout << "\nMAIN MENU\n";
        cout << "1. Image Processing\n";
        cout << "2. Audio Processing\n";
        cout << "3. Video Processing\n";
        cout << "4. View Processing History\n";
        cout << "5. Exit\n";

        cout << "\nEnter your choice: ";
        cin >> choice;

        if (choice >= 1 && choice <= 3) {

            cout << "\nEnter file name with extension: ";
            cin >> fileName;

            if (choice == 1) {
                Image img(fileName);
                img.process();

                processingQueue.push(
                    "Image processing request: " + fileName
                );
            }

            else if (choice == 2) {
                Audio aud(fileName);
                aud.process();

                processingQueue.push(
                    "Audio processing request: " + fileName
                );
            }

            else if (choice == 3) {
                Video vid(fileName);
                vid.process();

                processingQueue.push(
                    "Video processing request: " + fileName
                );
            }
        }

        else if (choice == 4) {

            cout << "\n--- PROCESSING HISTORY ---\n";

            if (processingQueue.empty()) {
                cout << "No processing requests yet.\n";
            }

            while (!processingQueue.empty()) {

                cout << processingQueue.front() << endl;
                processingQueue.pop();
            }
        }

        else if (choice == 5) {
            cout << "\nThank you for using Multimedia Processor!\n";
        }

        else {
            cout << "\nInvalid choice! Try again.\n";
        }

    } while (choice != 5);

    return 0;
}
