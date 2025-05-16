# 🎤 VOSK Speech Recognition in C++

This project demonstrates how to use the [VOSK](https://github.com/alphacep/vosk-api) speech recognition API in C++ to transcribe spoken words from a `.wav` audio file.

It uses:
- **VOSK API** for speech-to-text
- **libsndfile** to read `.wav` audio files

---

## 🚀 Features

- Offline speech recognition
- Simple and minimal C++ implementation
- Supports VOSK-compatible English models
- Works with mono, 16kHz WAV files

---

## 📦 Requirements

- Linux (tested on Arch/Ubuntu)
- C++17 compiler (e.g. `g++`)
- Git
- CMake
- `libsndfile` library

---

## 🔧 Installation & Setup

### 1. Clone this repository

```bash
git clone https://github.com/yourusername/vosk-cpp-demo.git
cd vosk-cpp-demo
2. Download and build VOSK API
bash
Zkopírovat
Upravit
git clone https://github.com/alphacep/vosk-api.git
cd vosk-api
make
cd ..
This builds libvosk.so which is required for linking.

3. Install required dependencies
On Arch:

bash
Zkopírovat
Upravit
sudo pacman -S libsndfile
On Ubuntu/Debian:

bash
Zkopírovat
Upravit
sudo apt update
sudo apt install libsndfile1-dev
4. Download a VOSK model
Download a small English model from:

https://alphacephei.com/vosk/models

For example:

bash
Zkopírovat
Upravit
wget https://alphacephei.com/vosk/models/vosk-model-small-en-us-0.15.zip
unzip vosk-model-small-en-us-0.15.zip
🛠️ Build the C++ code
bash
Zkopírovat
Upravit
g++ -std=c++17 -Ivosk-api -Lvosk-api -lvosk -lsndfile -o vosk_demo vosk_demo.cpp
This will create an executable vosk_demo.

▶️ Run
Make sure your audio file is in mono and 16kHz. If not, convert it:

bash
Zkopírovat
Upravit
ffmpeg -i input.wav -ar 16000 -ac 1 output.wav
Then run:

bash
Zkopírovat
Upravit
./vosk_demo ./vosk-model-small-en-us-0.15/ path/to/output.wav
Example output:

json
Zkopírovat
Upravit
{"text": "hello world"}
🧠 Notes
Only mono (1 channel) and 16,000 Hz WAV files are supported.

Recognition results are printed as JSON to the terminal.

For continuous microphone input, see VOSK Python or streaming C++ examples.

📂 Project Structure
bash
Zkopírovat
Upravit
vosk-cpp-demo/
├── vosk_demo.cpp       # Main C++ program
├── vosk-api/           # Cloned and built VOSK API
├── vosk-model-small-en-us-0.15/  # Speech model
└── README.md
📄 License
This project uses VOSK, licensed under Apache 2.0. See VOSK License.

🙋‍♂️ Questions?
Open an issue or visit https://github.com/alphacep/vosk-api for more documentation.

yaml
Zkopírovat
Upravit

---

Pokud budeš chtít i českou verzi README, dej vědět.
