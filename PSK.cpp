/**
 * @file PSK.cpp
 * @author Joshua Jerred (https://joshuajer.red)
 * @brief Implementation of BPSK and QPSK
 * @details 
 * This file contains a class 'PSK' which implements wav file
 * generation for both BPSK and QPSK. 
 * 
 * Usage:
 * Call constructor with file path, mode, and optional callsign.
 * Call encodeTextData(std::string message)
 * 
 * This will generate a wav file at the file path specified with PSK
 * audio data generated from the message and your specified mode.
 * 
 * Constructors:
 * It has two constructors, both 
 * require the file path and mode but one allows your to specify your call sign.
 * If you specify your callsign, it will be added (in morse code) to the
 * beginning and end of the wav file. If you do not specify your callsign
 * then it will not be included so it should be included in your message.
 * 
 * I make no guarantees that this fully complies with the public PSK31 
 * specifications (It currently does not!). Please take time to consider how 
 * to identify your station before transmitting any audio to prevent any
 * issues with the FCC or other operators in your area. 
 * 
 * Modes:
 * BPSK125, BPSK250, BPSK500
 * QPSK125, QPSK250, QPSK500
 * 
 * There currently is no support for the 31.25 or 62.5 baud modes, only
 * 125, 250, and 500 are supported. 
 * 
 * IMPORTANT:
 * There is **no** cosine filtering. Decoding in Fldigi is fully functional but 
 * the audio is ***not clean***.
 * 
 * Example:
 * PSK psk("test.wav", PSK::QPSK500, "KD9GDC");
 * psk.encodeTextData("Hello World!");
 * 
 * @date 2022-11-23
 * @copyright Copyright (c) 2022
 * @version 0.1
 */

#include "PSK.h"

#include <iostream> // Debugging
#include <bitset> // Debugging

/**
 * @brief Construct a PSK Modulator object without morse callsign.
 * 
 * @param file_path Path to where the wav file will be saved
 * @param mode BPSK125, BPSK250, BPSK500, QPSK125, QPSK250, QPSK500
 */
PSK::PSK(std::string file_path, Mode mode) {
    file_path_ = file_path;
    mode_ = mode;
    call_sign_ = "";
    morse_callsign_ = false;

    switch (mode_) {
        case BPSK125:
            symbol_rate_ = 125;
            break;
        case BPSK250:
            symbol_rate_ = 250;
            break;
        case BPSK500:
            symbol_rate_ = 500;
            break;
        case QPSK125:
            symbol_rate_ = 125;
            break;
        case QPSK250:
            symbol_rate_ = 250;
            break;
        case QPSK500:
            symbol_rate_ = 500;
            break;
        default:
            throw std::invalid_argument("Invalid mode");
            break;
    }

    samples_per_symbol_ = std::floor(sample_rate_ / symbol_rate_);
}

/**
 * @brief Construct a PSK Modulator object with morese code callsign added to 
 * beginning and end of wav file.
 * 
 * @param file_path Path to where the wav file will be saved
 * @param mode BPSK125, BPSK250, BPSK500, QPSK125, QPSK250, QPSK500
 * @param call_sign Callsign - must be at least 4 characters
 */
PSK::PSK(std::string file_path, Mode mode, std::string call_sign) {
    file_path_ = file_path;
    mode_ = mode;
    call_sign_ = call_sign;
    morse_callsign_ = true;

    switch (mode_) {
        case BPSK125:
            symbol_rate_ = 125;
            break;
        case BPSK250:
            symbol_rate_ = 250;
            break;
        case BPSK500:
            symbol_rate_ = 500;
            break;
        case QPSK125:
            symbol_rate_ = 125;
            break;
        case QPSK250:
            symbol_rate_ = 250;
            break;
        case QPSK500:
            symbol_rate_ = 500;
            break;
        default:
            throw std::invalid_argument("Invalid mode");
            break;
    }

    samples_per_symbol_ = std::floor(sample_rate_ / symbol_rate_);
}

PSK::~PSK() {

}

/**
 * @brief Encode a string of text data into PSK audio data and save to wave
 * file. 
 * @details Adds preamble and postamble to the audio data and if a callsign was
 * specified, adds morse code callsign to the beginning and end of the audio.
 * 
 * This method will use varicode with two 0's between each character.
 * If in QPSK mode, it will use the convolutional code.
 * 
 * @param message 
 * @return true - Success, you can find the wav file at the file path specified
 * @return false - Failure, check the console for more information
 */
bool PSK::encodeTextData(std::string message) {
    if (!openFile(file_path_)) { // Open Wav File at Path
        throw std::invalid_argument("Failed to open file at path: " + file_path_);
    } else {
        writeHeader(); // Write Wav Header on success
    }

    //if (morse_callsign_) { // If callsign specified, add morse code callsign
    //    if (call_sign_.length() < 4) {
    //        throw std::invalid_argument("Callsign must be at least 4 characters");
    //    }
    //    addCallSign();
    //}

    addPreamble(); // Add PSK Preamble to bitstream (0's)
    for (char &c : message) {
        addVaricode(c); // Add each char of the message to the bitstream (varicode)
        static unsigned char zeros[1] = {0x00};
        addBits(zeros, 2); // Add two 0's between each character
    }
    addPostamble(); // Add PSK Postamble to bitstream
    pushBufferToBitStream(); // Push any remaining bits in the buffer to the bitstream

    //encodeBitStream(); // Encode the bitstream into PSK audio, write to wav file

    //if (morse_callsign_) { // If callsign specified, add morse code callsign to end of audio
    //    addCallSign();
    //}

    //finalizeFile(); // Close wav file
    return true;
}

/**
 * @brief Encode raw data into PSK audio.
 * @details Adds preamble and postamble to the audio, and callsign if specified.
 * Does *not* use varicode or convolutional code! This should only be used
 * if you are manually decoding. You will be forced to use a morse callsign
 * if you use this method. You can of course change this if you want to, but
 * please keep in mind that if you use your own encoding people will not
 * be able to identify your station!
 * 
 * @param data 
 * @param length 
 * @return true 
 * @return false - Failure, check the console for more information.
 */
bool PSK::encodeRawData(unsigned char *data, int length) {
    if (!openFile(file_path_)) { // Open Wav File at Path
        throw std::invalid_argument("Failed to open file at path: " + file_path_);
    } else {
        writeHeader(); // Write Wav Header on success
    }

    if (!morse_callsign_) { // If callsign specified, add morse code callsign
        throw std::invalid_argument("Callsign required for raw data");
    }
    if (call_sign_.length() < 4) {
        throw std::invalid_argument("Callsign must be at least 4 characters");
    }

    addCallSign();

    addPreamble(); // Add PSK Preamble to bitstream
    //addBits(data, length*8); // Add raw data to bitstream
    addPostamble(); // Add PSK Postamble to bitstream
    encodeBitStream(); // Encode the bitstream into PSK audio, write to wav file

    addCallSign();

    finalizeFile(); // Close wav file
    return true;
}

void PSK::dumpBitStream() {
    std::cout << "Bitstream:" << std::endl;
    for (int i = 0; i < bit_stream_.size(); i++) {
        std::cout << "[" << i << "]" << std::bitset<32>(bit_stream_[i]) << std::endl;
    }
}

// Private Methods

/**
 * @brief Opens the wav file and writes the header.
 * @param file_path 
 * @return true - Success
 * @return false - Failure
 */
bool PSK::openFile(std::string file_path) {
    wav_file_.open(file_path, std::ios::binary);
    if (wav_file_.is_open()) {
        return true;
    } else {
        return false;
    }
}

/**
 * @brief Writes the header to the wav file, this happens before audio data.
 * @details This method will save the location of the 'data size' field in
 * the header so that it can be updated later after adding data.
 */
void PSK::writeHeader() {
    wav_file_ << "RIFF****WAVE"; // RIFF header
    wav_file_ << "fmt "; // format
    writeBytes(16, 4); // size
    writeBytes(1, 2); // compression code
    writeBytes(1, 2); // number of channels
    writeBytes(sample_rate_, 4); // sample rate
    writeBytes(sample_rate_ * bits_per_sample_ / 8, 4 ); // Byte rate
    writeBytes(bits_per_sample_ / 8, 2); // block align
    writeBytes(bits_per_sample_, 2); // bits per sample
    wav_file_ << "data****"; // data section follows this

    // Save the location of the data size field so that it can be updated later
    data_start_ = wav_file_.tellp();
}

/**
 * @brief Writes the audio data to the wav file.
 * @param data - int of data to write to wav file
 * @param num_bytes - number of bytes to write
 */
void PSK::writeBytes(int data, int size) {
    wav_file_.write(reinterpret_cast<const char*> (&data), size);
}

/**
 * @brief Updates the 'data segment size' field in the wav header and closes
 * the file.
 */
void PSK::finalizeFile() {
    int data_end_ = wav_file_.tellp(); // Save the position of the end of the 
                                       // data chunk
    wav_file_.seekp(data_start_ - 4); // Go to the beginning of the data chunk
    writeBytes(data_end_ - data_start_, 4); // and write the size of the chunk.
    wav_file_.seekp(4, std::ios::beg); // Go to the beginning of the file
    writeBytes(data_end_ - 8, 4); // Write the size of the overall file
    wav_file_.close();
}

/**
 * @brief Adds specified callsign to the wav file (morse code)
 * 
 */
void PSK::addCallSign() {

}

void PSK::addVaricode(char c) {
    uint16_t varicode = ascii_to_varicode[c];
    //std::cout << std::endl << "Adding varicode for " << c << ":" << std::bitset<16>(varicode) << " ";
    unsigned char bits[2];
    int previous_bit = 1;
    for (int i = 0; i < 16; i++) {
        if (!(varicode & (1 << i)) && (!previous_bit)) {
            //std::cout << "size: " << i - 1;

            bits[0] = i > 9 ? varicode >> 1 : varicode << (9 - i);
            bits[1] = i > 9 ? varicode << (17 - i) : 0x00;

            //std::cout << " shifted: " << std::bitset<8>(bits[0]) << " " << std::bitset<8>(bits[1]) << std::endl;
            addBits(bits, i - 1);
            break;
        } else {
            previous_bit = varicode & (1 << i);
        }
    }
}

// Bit Stream Methods
/**
 * @brief Adds a bit to the bit stream.
 * @details see format of 'bit stream' in header file. This function assumes
 * that you're adding bits in order (left to right). So if you want to add 1
 * you must add 0x80 with a 'num_bits' of 1. All numbers past 'num_bits'
 * must be 0.
 * @param data 
 * @param num_bits 
 */
void PSK::addBits(unsigned char *data, int num_bits) {
    int data_index = 0;
    int bit_index = 0; // Index of the bit in the byte, left to right [0 - 7]
    int buffer_space;
    
    while (num_bits > 0) {
        buffer_space = 32 - bit_stream_offset_;
        
        if (buffer_space == 0) { // buffer is full, write to bit stream
            //std::cout << "Buffer full, writing to bit stream" << std::endl;
            bit_stream_.push_back(bit_stream_buffer_);
            bit_stream_buffer_ = 0;
            bit_stream_offset_ = 0;
            buffer_space = 32;
        }

        if (buffer_space >= num_bits && num_bits > 8) { // Write a byte at a time
            //std::cout << "Writing a byte at a time" << std::endl;
            bit_stream_buffer_ |= (data[data_index] << (32 - bit_stream_offset_ - 8));
            bit_stream_offset_ += 8;
            num_bits -= 8;
            data_index++;
        } else { // Write a bit at a time
            //std::cout << "Writing a bit at a time" << std::endl;
            bit_stream_buffer_ |= (data[data_index] & (1 << (7 - bit_index))) ? 1 << (32 - bit_stream_offset_ - 1) : 0;
            bit_stream_offset_++;
            num_bits--;
            bit_index++;
            if (bit_index == 8) {
                bit_index = 0;
                data_index++;
            }
        }

        //std::cout << "Buffer: " << std::bitset<32>(bit_stream_buffer_) << std::endl;
        //for (int i = 0; i < bit_stream_.size(); i++) {
        //    std::cout << "[" << i << "] " << std::bitset<32>(bit_stream_[i]) << std::endl;
        //}
    }
}

/**
 * @brief After adding all of the data to the bit stream, this method will
 * write the data left in the buffer 
 * 
 */
void PSK::pushBufferToBitStream() {
    bit_stream_.push_back(bit_stream_buffer_);
    bit_stream_buffer_ = 0;
    bit_stream_offset_ = 0;
}

/**
 * @brief Adds the preamble to the bit stream.
 */
void PSK::addPreamble() {
    static unsigned char zeros[1] = {0x00};
    for (int i = 0; i < preamble_length_; i++) {
        addBits(zeros, 1);
    }
}

/**
 * @brief Adds the postamble to the bit stream.
 */
void PSK::addPostamble() {
    static unsigned char zeros[1] = {0x00};
    for (int i = 0; i < postamble_length_; i++) {
        addBits(zeros, 1);
    }
}

/**
 * @brief Returns the next bit of the bit stream that will be encoded.
 * 
 * @return int [1, 0, -1] -1 = no more bits in bit stream
 */
int PSK::popNextBit() {
    return -1;
}

// Modulation Methods
/**
 * @brief Goes throught he bit stream and modulates with the addSymbol method.
 */
void PSK::encodeBitStream() {

}

/**
 * @brief Modulates a single symbol in BPSK/QPSK and saves the audio data to the
 * wav file.
 * 
 * @param shift 
 */
void PSK::addSymbol(int shift) {
    for (int i = 0; i < samples_per_symbol_; i++) {
        int sample = 
    }
}

int main() {
    std::string message = "Hello World!";
    PSK psk("test.wav", PSK::Mode::BPSK125);
    psk.encodeTextData(message);
    psk.dumpBitStream();
    return 0;
}