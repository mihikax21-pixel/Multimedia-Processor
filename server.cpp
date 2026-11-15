
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <filesystem>
#include <cctype>
#include <sstream>
#include <iomanip>
#include "httplib.h"

using namespace std;
namespace fs = std::filesystem;

const string FFMPEG = "ffmpeg";

int main() {

    httplib::Server server;

    fs::create_directories("uploads");
    fs::create_directories("outputs");

    // CORS
    server.set_post_routing_handler(
        [](const httplib::Request& req, httplib::Response& res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "POST, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
        }
    );

    server.Options("/process",
        [](const httplib::Request& req, httplib::Response& res) {
            res.status = 200;
        }
    );

    // PROCESS MEDIA
    server.Post("/process",
        [](const httplib::Request& req, httplib::Response& res) {

        if (!req.form.has_file("file")) {
            res.status = 400;
            res.set_content("No file uploaded", "text/plain");
            return;
        }

        string type = req.form.get_field("type");
        string operation = req.form.get_field("operation");

        const auto& file = req.form.get_file("file");

        // Get file extension
        string ext = fs::path(file.filename).extension().string();

        if (ext.empty()) {
            res.status = 400;
            res.set_content("Invalid file extension", "text/plain");
            return;
        }

        for (char& c : ext) {
            c = static_cast<char>(
                tolower(static_cast<unsigned char>(c))
            );
        }

        // Validate media type
        if (type != "image" && type != "audio" && type != "video") {
            res.status = 400;
            res.set_content("Invalid media type", "text/plain");
            return;
        }

        // Save uploaded file
        string input = "uploads/input" + ext;

        ofstream uploaded(input, ios::binary);

        if (!uploaded) {
            res.status = 500;
            res.set_content("Unable to save uploaded file", "text/plain");
            return;
        }

        uploaded.write(file.content.data(), file.content.size());
        uploaded.close();

        // Output format
        string outputExt = ext;

        if (type == "image") {
            if (ext == ".jpg" || ext == ".jpeg") {
                outputExt = ".jpg";
            } else if (ext == ".webp") {
                outputExt = ".webp";
            } else {
                outputExt = ".png";
            }
        }

        if (type == "audio") {
            if (ext == ".wav") outputExt = ".wav";
            else if (ext == ".ogg") outputExt = ".ogg";
            else outputExt = ".mp3";
        }

        if (type == "video") {
            outputExt = ".mp4";
        }

        string output = "outputs/processed" + outputExt;

        string command;

        // ================= IMAGE =================

        if (type == "image") {

            if (operation == "Resize Image") {

                int width, height;

                try {
                    width = stoi(req.form.get_field("width"));
                    height = stoi(req.form.get_field("height"));

                    if (width <= 0 || height <= 0 ||
                        width > 5000 || height > 5000) {
                        res.status = 400;
                        res.set_content("Invalid image dimensions", "text/plain");
                        return;
                    }
                }
                catch (...) {
                    res.status = 400;
                    res.set_content("Invalid image dimensions", "text/plain");
                    return;
                }

                command = FFMPEG + " -y -i \"" + input +
                    "\" -vf scale=" + to_string(width) + ":" +
                    to_string(height) + " \"" + output + "\"";
            }

            else if (operation == "Convert to Grayscale") {

                command = FFMPEG + " -y -i \"" + input +
                    "\" -vf format=gray \"" + output + "\"";
            }

            else {
                res.status = 400;
                res.set_content("Unsupported image operation", "text/plain");
                return;
            }
        }

        // ================= AUDIO =================

        else if (type == "audio") {

            if (operation == "Adjust Volume") {

                command = FFMPEG + " -y -i \"" + input +
                    "\" -filter:a volume=1.5 \"" + output + "\"";
            }

            else if (operation == "Trim Audio") {

                double duration;

                try {
                    duration = stod(req.form.get_field("duration"));

                    if (duration <= 0 || duration > 3600) {
                        res.status = 400;
                        res.set_content("Invalid audio duration", "text/plain");
                        return;
                    }
                }
                catch (...) {
                    res.status = 400;
                    res.set_content("Invalid audio duration", "text/plain");
                    return;
                }

                command = FFMPEG + " -y -i \"" + input +
                    "\" -t " + to_string(duration) +
                    " -vn \"" + output + "\"";
            }

            else {
                res.status = 400;
                res.set_content("Unsupported audio operation", "text/plain");
                return;
            }
        }

        // ================= VIDEO =================

        else if (type == "video") {

            if (operation == "Trim Video") {

                double duration;

                try {
                    duration = stod(req.form.get_field("duration"));

                    if (duration <= 0 || duration > 3600) {
                        res.status = 400;
                        res.set_content("Invalid video duration", "text/plain");
                        return;
                    }
                }
                catch (...) {
                    res.status = 400;
                    res.set_content("Invalid video duration", "text/plain");
                    return;
                }

                command = FFMPEG + " -y -i \"" + input +
                    "\" -t " + to_string(duration) +
                    " -c:v libx264 -preset veryfast "
                    "-c:a aac -movflags +faststart \"" +
                    output + "\"";
            }

            else if (operation == "Video Compression") {

                command = FFMPEG + " -y -i \"" + input +
                    "\" -c:v libx264 -crf 28 "
                    "-preset veryfast -c:a aac \"" +
                    output + "\"";
            }

            else {
                res.status = 400;
                res.set_content("Unsupported video operation", "text/plain");
                return;
            }
        }

        // ================= RUN FFMPEG =================

        if (fs::exists(output)) {
            fs::remove(output);
        }

        cout << "\nProcessing: " << type
             << " | " << operation << endl;

        cout << "FFmpeg command: " << command << endl;

        int result = system(command.c_str());

        if (result != 0 || !fs::exists(output) ||
            fs::file_size(output) == 0) {

            res.status = 500;
            res.set_content("FFmpeg processing failed. Check terminal.",
                            "text/plain");
            return;
        }

        // ================= SEND TO FRONTEND =================

        ifstream processed(output, ios::binary);

        if (!processed) {
            res.status = 500;
            res.set_content("Unable to read processed file", "text/plain");
            return;
        }

        string data(
            (istreambuf_iterator<char>(processed)),
            istreambuf_iterator<char>()
        );

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Content-Disposition",
                       "attachment; filename=processed" + outputExt);

        res.set_content(data, "application/octet-stream");

        cout << "Processing successful!" << endl;
    });

    cout << "Multimedia Processor server running at "
         << "http://127.0.0.1:8080" << endl;

    server.listen("127.0.0.1", 8080);

    return 0;
}
