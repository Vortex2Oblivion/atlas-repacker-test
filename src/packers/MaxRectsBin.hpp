#pragma once

#include "Packer.hpp"

namespace packers {
    class MaxRectsBin : public Packer {
        public:
            MaxRectsBin(uint16_t width, uint16_t height);
            ~MaxRectsBin();
            
            Rectangle insert(uint16_t width, uint16_t height);
        private:

        	size_t newFreeRectanglesLastSize = 0;
	        std::vector<Rectangle> newFreeRectangles = {};
            std::vector<Rectangle> usedRectangles = {};

            void placeRect(const Rectangle& node);
            void insertNewFreeRectangle(const Rectangle &newFreeRect);
            void pruneFreeList();
            bool splitFreeNode(const Rectangle &freeNode, const Rectangle &usedNode);
            Rectangle bestNodeShortSideFit(uint16_t width, uint16_t height, uint16_t &bestShortSideFit, uint16_t &bestLongSideFit) const;
    };
}  // namespace packers