#pragma once

#include "Packer.hpp"

namespace packers {
    class MaxRectsBin : public Packer {
        public:
            MaxRectsBin(float width, float height);
            ~MaxRectsBin();
            
            Rectangle insert(float width, float height);
        private:

        	size_t newFreeRectanglesLastSize;
	        std::vector<Rectangle> newFreeRectangles = {};
            std::vector<Rectangle> usedRectangles = {};

            void placeRect(const Rectangle& node);
            void insertNewFreeRectangle(const Rectangle &newFreeRect);
            void pruneFreeList();
            bool splitFreeNode(const Rectangle &freeNode, const Rectangle &usedNode);
            Rectangle bestNodeShortSideFit(int width, int height, int &bestShortSideFit, int &bestLongSideFit);
    };
}  // namespace packers