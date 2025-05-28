/**
 * @file SD.h
 * @author Phil Schatzmann
 * @brief The most important functions of SD library implemented based on std
 * @version 0.1
 * @date 2022-02-07
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once

#include <sys/stat.h>
#include <sys/types.h>

#include <fstream>
#include <iostream>
#include <string>

#include "Stream.h"
#ifdef USE_FILESYSTEM
#include <filesystem>
#endif

#define SD_SCK_MHZ(maxMhz) (1000000UL * (maxMhz))
#define SPI_FULL_SPEED SD_SCK_MHZ(50)
#define SPI_DIV3_SPEED SD_SCK_MHZ(16)
#define SPI_HALF_SPEED SD_SCK_MHZ(4)
#define SPI_DIV6_SPEED SD_SCK_MHZ(8)
#define SPI_QUARTER_SPEED SD_SCK_MHZ(2)
#define SPI_EIGHTH_SPEED SD_SCK_MHZ(1)
#define SPI_SIXTEENTH_SPEED SD_SCK_HZ(500000)

#define O_RDONLY ios::in           ///< Open for reading only.
#define O_WRONLY ios::out          ///< Open for writing only.
#define O_RDWR ios::in | ios::out  ///< Open for reading and writing.
#define O_AT_END ios::ate          ///< Open at EOF.
#define O_APPEND ios::ate          ///< Set append mode.
#define O_CREAT ios::trunc         ///< Create file if it does not exist.
#define O_TRUNC ios::trunc         ///< Truncate file to zero length.
#define O_EXCL 0                   ///< Fail if the file exists.
#define O_SYNC 0                   ///< Synchronized write I/O operations.
#define O_READ O_RDONLY
#define O_WRITE O_WRONLY
#define FILE_READ ios::in
#define FILE_WRITE ios::out

#ifndef SS
#define SS 0
#endif

using namespace std;

/**
 * @brief C++ std based emulatoion of SdSpiConfig
 *
 */

class SdSpiConfig {
  SdSpiConfig(int a = 0, int b = 0, int c = 0) {}
};

/**
 * @brief C++ std based emulatoion ofr File
 *
 */
class File : public Stream {
 public:
  bool isOpen() {
    bool result = file.is_open();
    return result;
  }

  bool open(const char *name, int flags) {
    this->filename = name;
    int rc = stat(name, &info);
    if ((flags == O_RDONLY) && rc == -1) {
      // if we want to read but the file does not exist we fail
      return false;
    } else if (rc == 0 && info.st_mode & S_IFDIR) {
      // file exists and it is a directory
      is_dir = true;
      size_bytes = 0;
#ifdef USE_FILESYSTEM
      dir_path = std::filesystem::path(filename);
      iterator = std::filesystem::directory_iterator({dir_path});
#endif
      return true;
    } else {
      is_dir = false;
      size_bytes = rc != -1 ? info.st_size : 0;
      file.open(filename.c_str(), flags);
      return isOpen();
    }
  }

  bool close() {
    file.close();
    return !isOpen();
  }

  size_t readBytes(char *buffer, size_t length) override {
    return file.readsome((char *)buffer, length);
  }

  size_t write(const uint8_t *buffer, size_t size) override {
    file.write((const char *)buffer, size);
    return file.good() ? size : 0;
  }
  size_t write(uint8_t ch) override {
    file.put(ch);
    return file.good() ? 1 : 0;
  }
  int available() override { return size_bytes - file.tellg(); };
  int availableForWrite() override { return 1024; }
  int read() override { return file.get(); }
  int peek() override { return file.peek(); }
  void flush() override {}
  void getName(char *str, size_t len) { strncpy(str, filename.c_str(), len); }
  bool isDir() { return is_dir; }
  bool isHidden() { return false; }
  bool rewind() {
    pos = 0;
    return is_dir;
  }
  bool openNext(File &dir, int flags = O_RDONLY) {
#ifdef USE_FILESYSTEM
    if (dir.isDir() && dir.iterator != end(dir.iterator)) {
      std::filesystem::directory_entry entry = *dir.iterator++;
      return open(entry.path().c_str(), flags);
    }
#endif
    return false;
  }
  int dirIndex() { return pos; }

  size_t size() { 
    int rc = stat(filename.c_str(), &info);
    size_bytes = rc != -1 ? info.st_size : 0;
    return size_bytes;
  }

  size_t position() { return file.tellg(); }
  size_t seek(size_t pos) {
    file.seekg(pos);
    return pos;
  }

  operator boolean() { return isOpen(); }

 protected:
  std::fstream file;
  size_t size_bytes = 0;
  bool is_dir;
  int pos = 0;
  std::string filename;
  struct stat info;

#ifdef USE_FILESYSTEM
  std::filesystem::directory_iterator iterator;
  std::filesystem::path dir_path;
#endif
};

/**
 * @brief C++ std based emulatoion ofr SdFat
 *
 */
class SdFat {
 public:
  bool begin(int cs = SS, int speed = 0) { return true; }
  bool begin(SdSpiConfig &cfg) { return true; }
  void errorHalt(const char *msg) {
    Serial.println(msg);
    exit(0);
  }
  void initErrorHalt() { exit(0); }

  bool exists(const char *name) {
    struct stat info;
    return stat(name, &info) == 0;
  }

  File open(const char *name, int flags) {
    File file;
    file.open(name, flags);
    return file;
  }

  bool remove(const char *name) { return std::remove(name) == 0; }
  bool mkdir(const char *name) { return ::mkdir(name,0777) == 0; }
};

static SdFat SD;
