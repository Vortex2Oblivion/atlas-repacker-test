#include "Exporter.hpp"

#include "pugixml.hpp"

#include <filesystem>

Exporter::Exporter() = default;

Exporter::~Exporter() = default;

void Exporter::exportToFile(const std::string& filePath, const Image &image) const {
	if (!ExportImage(image, (filePath + ".png").c_str())) {
		return;
	}

	pugi::xml_document doc;
	doc.load_string((R"(
		<?xml version="1.0" encoding"utf-8"?>
		<TextureAtlas imagePath=)" + std::filesystem::path(filePath + ".xml").filename().string() + ">"
	).c_str());
	for (const auto& frame: frames) {
		auto node = doc.child("TextureAtlas").append_child("Texture");
		node.append_attribute("name");
		node.attribute("name").set_value(frame.name.c_str());
	}
	if (!doc.save_file((filePath + ".xml").c_str())) {
		TraceLog(LOG_ERROR, ("Failed to export xml to path " + filePath + ".xml").c_str());
	}
}
