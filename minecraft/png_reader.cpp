#include "png_reader.h"

#include <png.h>

uint8_t* read_png(const char* filename, int* w, int* h) {
	return read_png(filename, w, h, false);
}

uint8_t* read_png(const char* filename, int* w, int* h, bool flip_vertically) {
	// Open file
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		std::cerr << "Can't open png file" << std::endl;
		return nullptr;
	}
	// Read image and return data
	uint8_t* image_data = _read_png(fp, (uint32_t*)w, (uint32_t*)h, flip_vertically);
	fclose(fp);
	return image_data;
}

uint8_t* _read_png(FILE* fp, uint32_t* w, uint32_t* h, bool flip_vertically) {
	// Check PNG signature
	uint8_t sig[8];
	fread(sig, 1, 8, fp);
	if (!png_check_sig(sig, 8)) {
		std::cerr << "Bad png signature" << std::endl;
		return nullptr;
	}

	// Get PNG structs
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png) {
		std::cerr << "Out of memory" << std::endl;
		return nullptr;
	}
	png_infop info = png_create_info_struct(png);
	if (!info) {
		std::cerr << "Out of memory" << std::endl;
		png_destroy_read_struct(&png, NULL, NULL);
		return nullptr;
	}

	// Libpng error handler
	if (setjmp(png_jmpbuf(png))) {
		png_destroy_read_struct(&png, &info, NULL);
		return nullptr;
	}

	// Get info about image
	png_init_io(png, fp);
	png_set_sig_bytes(png, 8);
	png_read_info(png, info);

	int bit_depth, color_type;
	png_get_IHDR(png, info, w, h, &bit_depth, &color_type, NULL, NULL, NULL);

	// Transformations
	/// Expand data if necessary
	/* Older approach, the newer one is below */
	/*if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand(png);
	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_expand(png);*/
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png);
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png);
	if (png_get_valid(png, info, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	/// Reduce 16 bit samples to 8 bit samples
	if (bit_depth == 16)
		png_set_strip_16(png);

	/// If color type doesn't have an alpha channel, fill it with 255
	if (color_type == PNG_COLOR_TYPE_RGB ||
		color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_PALETTE
	)
		png_set_filler(png, 0xFF, PNG_FILLER_AFTER);

		/// Convert grayscale to RGB
	if (color_type == PNG_COLOR_TYPE_GRAY ||
		color_type == PNG_COLOR_TYPE_GRAY_ALPHA
		)
		png_set_gray_to_rgb(png);

	// Update info
	png_read_update_info(png, info);
	size_t rowbytes = png_get_rowbytes(png, info);
	uint8_t channels = png_get_channels(png, info);

	// Read image data
	uint8_t* image_data = new (std::nothrow) uint8_t[rowbytes * (*h)];
	if (!image_data) {
		std::cerr << "Can't allocate image data" << std::endl;
		png_destroy_read_struct(&png, &info, NULL);
		return nullptr;
	}
	uint8_t** row_pointers = new (std::nothrow) uint8_t*[(*h)];
	if (!row_pointers) {
		std::cerr << "Can't allocate row pointers" << std::endl;
		png_destroy_read_struct(&png, &info, NULL);
		delete[] image_data;
		return nullptr;
	}
	for (unsigned int i = 0; i < (*h); ++i)
		row_pointers[i] = image_data + (flip_vertically ? (*h) - 1 - i : i) * rowbytes;
	png_read_image(png, row_pointers);

	// Cleanup and finish
	delete[] row_pointers;
	png_read_end(png, NULL); // Can be omitted
	png_destroy_read_struct(&png, &info, NULL);

	return image_data;
}

void free_png(uint8_t* image_data) {
	delete[] image_data;
}
