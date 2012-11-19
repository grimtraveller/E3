
/****************************************************
 Bridge portsf/AudioFile
*****************************************************/

#pragma once

#include <string>
#include <stdexcept>

#include "portsf.h"

#include <Macros.h>

#include <core/AudioFile.h>



class PortSfFile : public AudioFile
{
public:
	PortSfFile();
	~PortSfFile();

	void open(const Path& filename, FileOpenMode mode);
	void close();
	int64 seek(int64 frame);

	int64 readShort(short* buffer, int64 num)       { EXCEPTION(std::exception, "not implemented"); }
    int64 readInt(int* buffer, int64 num)           { EXCEPTION(std::exception, "not implemented"); }
    int64 readFloat(float* buffer, int64 num);
    int64 readDouble(double* buffer, int64 num);

    int64 writeShort(short* buffer, int64 num)      { EXCEPTION(std::exception, "not implemented"); }
    int64 writeInt(int* buffer, int64 num)          { EXCEPTION(std::exception, "not implemented"); }
    int64 writeFloat(float* buffer, int64 num)      { EXCEPTION(std::exception, "not implemented"); }
    int64 writeDouble(double* buffer, int64 num)    { EXCEPTION(std::exception, "not implemented"); }

    bool isOpen() const                             { return handle_ > -1; }
    bool hasError() const                           { EXCEPTION(std::exception, "not implemented"); }
    std::string getErrorString() const;
    std::string getVersionString() const            { return "portsf"; }

protected:
    typedef int Handle;
	Handle handle_;

private:
    /*! Helper class to perform static intialization */
    class Initializer {
    public:
        Initializer();
        ~Initializer();
    };
    static Initializer initializer_s;

    typedef EnumMapper<Format, psf_format> FormatMapper;
    static FormatMapper formatMapper_s;

    typedef EnumMapper<Encoding, psf_stype> EncodingMapper;
    static EncodingMapper encodingMapper_s;
};

