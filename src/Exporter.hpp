#pragma once

#include <vector>

#include "Frame.hpp"
#include "raylib.h"


class Exporter {
	public:
		Exporter();
		~Exporter();

		std::vector<Frame> frames = {};

		void exportToFile(const std::string& filePath, const Image &image) const;
};
