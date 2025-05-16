#include <iostream>
#include <string>
#include <cstdlib>
#include <uuid/uuid.h>
#include <curl/curl.h>
#include <json/json.h>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>

// VOSK & PortAudio
#include <vosk_api.h>
#include <portaudio.h>

#define SAMPLE_RATE 16000
#define FRAMES_PER_BUFFER 4000

// Onyx kontext
const std::string OLLAMA_URL = "http://192.168.1.65:11434/api/generate";
const std::string MODEL_NAME = "phi3";
const std::string ASSISTANT_NAME = "Onyx";
const std::string VOSK_MODEL_PATH = "/home/merix/vosk-model-small-en-us-0.15";

// CURL callback
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// VOSK: hlasové vstupy
std::string listen_with_vosk(const char* model_path) {
    if (Pa_Initialize() != paNoError) {
        std::cerr << "PortAudio initialization failed." << std::endl;
        return "";
    }

    PaStream* stream = nullptr;
    if (Pa_OpenDefaultStream(&stream, 1, 0, paInt16, SAMPLE_RATE, FRAMES_PER_BUFFER, nullptr, nullptr) != paNoError) {
        std::cerr << "Failed to open PortAudio stream." << std::endl;
        Pa_Terminate();
        return "";
    }

    VoskModel* model = vosk_model_new(model_path);
    if (!model) {
        std::cerr << "❌ Failed to load VOSK model." << std::endl;
        Pa_Terminate();
        return "";
    }

    VoskRecognizer* recognizer = vosk_recognizer_new(model, SAMPLE_RATE);
    if (!recognizer) {
        std::cerr << "❌ Failed to create recognizer." << std::endl;
        vosk_model_free(model);
        Pa_Terminate();
        return "";
    }

    Pa_StartStream(stream);
    std::cout << "🎙️ Listening..." << std::endl;

    short buffer[FRAMES_PER_BUFFER];
    std::string result_text;

    while (true) {
        PaError err = Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);
        if (err && err != paInputOverflowed) {
            std::cerr << "Error reading audio stream." << std::endl;
            break;
        }

        if (vosk_recognizer_accept_waveform(recognizer, buffer, FRAMES_PER_BUFFER * sizeof(short))) {
            const char* result = vosk_recognizer_result(recognizer);
            Json::Value root;
            Json::CharReaderBuilder reader;
            std::string errs;
            std::istringstream s(result);
            if (Json::parseFromStream(reader, s, &root, &errs)) {
                result_text = root["text"].asString();
                if (!result_text.empty()) {
                    break;  // hotová věta
                }
            }
        }
    }

    Pa_StopStream(stream);
    Pa_CloseStream(stream);
    Pa_Terminate();

    vosk_recognizer_free(recognizer);
    vosk_model_free(model);

    return result_text;
}

// Zabalené do rozhraní Onyx
std::string listen() {
    std::string result = listen_with_vosk(VOSK_MODEL_PATH.c_str());
    if (!result.empty())
        std::cout << "👤 You: " << result << std::endl;
    return result;
}

// Onyx mluví
void speak(const std::string& text) {
    std::cout << "🤖 " << ASSISTANT_NAME << ": " << text << std::endl;
    std::string command = "espeak \"" + text + "\"";
    system(command.c_str());
}

// Dotaz na Ollamu
std::string ask_ollama(const std::string& prompt) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    Json::Value payload;
    payload["model"] = MODEL_NAME;
    payload["prompt"] = prompt;
    payload["stream"] = false;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        std::string payloadStr = payload.toStyledString();
        curl_easy_setopt(curl, CURLOPT_URL, OLLAMA_URL.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();

    Json::CharReaderBuilder reader;
    Json::Value response;
    std::istringstream s(readBuffer);
    std::string errs;
    Json::parseFromStream(reader, s, &response, &errs);

    if (!response["response"].isNull()) {
        return response["response"].asString();
    } else {
        return "Sorry, I couldn't connect to my AI brain.";
    }
}

// Hlavní funkce
int main() {
    speak("Hello, I'm " + ASSISTANT_NAME + ", your AI assistant.");
    while (true) {
        std::string query = listen();
        if (!query.empty()) {
            if (query == "exit" || query == "quit" || query == "stop") {
                speak("Goodbye!");
                break;
            }
            std::string reply = ask_ollama(query);
            speak(reply);
        }
    }
    return 0;
}
