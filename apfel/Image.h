#pragma once

#include <memory>

class Image
{
public:
	explicit Image (arguments);	// type conversion
	Image();									// default ctor
	Image(const Image&);							// copy ctor
	Image(Image&&);								// move ctor
	Image& operator=(const Image&);				// copy assignment
	Image& operator=(Image&&);					// move assignment
	virtual ~Image ();

private:
	unsigned width_;
	unsigned height_;
	std::unique_ptr<unsigned char[]> data_;
};