#include "MaxRectsBin.hpp"

#include <climits>
#include <cstdlib>
#include <algorithm>

namespace packers {
    // based on https://github.com/juj/RectangleBinPack/blob/master/MaxRectsBinPack.cpp
    MaxRectsBin::MaxRectsBin(const uint16_t width, const uint16_t height) : Packer(width, height) {}

    MaxRectsBin::~MaxRectsBin() = default;

    Rectangle MaxRectsBin::insert(const uint16_t width, const uint16_t height){
        uint16_t score1 = UINT16_MAX;
        uint16_t score2 = UINT16_MAX;

        const Rectangle node = bestNodeShortSideFit(width, height, score1, score2);

        if(node.height == 0){
            return node;
        }

        placeRect(node);

        return node;
    }

    void MaxRectsBin::placeRect(const Rectangle& node){
        for(size_t i = 0; i < freeRectangles.size();){
            if(!splitFreeNode(freeRectangles[i], node)){
                i++;
                continue;
            }

            freeRectangles[i] = freeRectangles.back();
			freeRectangles.pop_back();
        }

        usedRectangles.push_back(node);
        pruneFreeList();
    } 

    bool MaxRectsBin::splitFreeNode(const Rectangle &freeNode, const Rectangle &usedNode){
	    if (usedNode.x >= freeNode.x + freeNode.width || usedNode.x + usedNode.width <= freeNode.x ||
                usedNode.y >= freeNode.y + freeNode.height || usedNode.y + usedNode.height <= freeNode.y){
		    return false;
        
        }
        
        newFreeRectanglesLastSize = newFreeRectangles.size();

        if (usedNode.x < freeNode.x + freeNode.width && usedNode.x + usedNode.width > freeNode.x){
            if (usedNode.y > freeNode.y && usedNode.y < freeNode.y + freeNode.height){
                Rectangle newNode = freeNode;
                newNode.height = usedNode.y - newNode.y;
                insertNewFreeRectangle(newNode);
            }

            if (usedNode.y + usedNode.height < freeNode.y + freeNode.height){
                Rectangle newNode = freeNode;
                newNode.y = usedNode.y + usedNode.height;
                newNode.height = freeNode.y + freeNode.height - (usedNode.y + usedNode.height);
                insertNewFreeRectangle(newNode);
            }
        }

        if (usedNode.y < freeNode.y + freeNode.height && usedNode.y + usedNode.height > freeNode.y){
            if (usedNode.x > freeNode.x && usedNode.x < freeNode.x + freeNode.width){
                Rectangle newNode = freeNode;
                newNode.width = usedNode.x - newNode.x;
                insertNewFreeRectangle(newNode);
            }

            if (usedNode.x + usedNode.width < freeNode.x + freeNode.width){
                Rectangle newNode = freeNode;
                newNode.x = usedNode.x + usedNode.width;
                newNode.width = freeNode.x + freeNode.width - (usedNode.x + usedNode.width);
                insertNewFreeRectangle(newNode);
            }
        }

        return true;
    }

    bool isContainedIn(const Rectangle &a, const Rectangle &b){
        return a.x >= b.x && a.y >= b.y 
            && a.x+a.width <= b.x+b.width 
            && a.y+a.height <= b.y+b.height;
    }

    void MaxRectsBin::insertNewFreeRectangle(const Rectangle &newFreeRect) {
        for(size_t i = 0; i < newFreeRectanglesLastSize;)
        {
            // This new free rectangle is already accounted for?
            if (isContainedIn(newFreeRect, newFreeRectangles[i]))
                return;

            // Does this new free rectangle obsolete a previous new free rectangle?
            if (isContainedIn(newFreeRectangles[i], newFreeRect))
            {
                // Remove i'th new free rectangle, but do so by retaining the order
                // of the older vs newest free rectangles that we may still be placing
                // in calling function SplitFreeNode().
                newFreeRectangles[i] = newFreeRectangles[--newFreeRectanglesLastSize];
                newFreeRectangles[newFreeRectanglesLastSize] = newFreeRectangles.back();
                newFreeRectangles.pop_back();
            }
            else 
                ++i;
        }
        newFreeRectangles.push_back(newFreeRect);
    }


    Rectangle MaxRectsBin::bestNodeShortSideFit(const uint16_t width, const uint16_t height, uint16_t &bestShortSideFit, uint16_t &bestLongSideFit) const {
        Rectangle bestNode = {};

        bestShortSideFit = UINT16_MAX;
        bestLongSideFit = UINT16_MAX;

        for(auto rectangle : freeRectangles){
            if(rectangle.width >= width && rectangle.height >= height){
                uint16_t leftoverH = abs(static_cast<uint16_t>(rectangle.width) - width);
                uint16_t leftoverV = abs(static_cast<uint16_t>(rectangle.height) - height);
                const uint16_t shortSideFit = std::min(leftoverH, leftoverV);
			    const uint16_t longSideFit = std::max(leftoverH, leftoverV);

                if (shortSideFit < bestShortSideFit || (shortSideFit == bestShortSideFit && longSideFit < bestLongSideFit)){
                    bestNode.x = rectangle.x;
                    bestNode.y = rectangle.y;
                    bestNode.width = width;
                    bestNode.height = height;
                    bestShortSideFit = shortSideFit;
                    bestLongSideFit = longSideFit;
                }
            }
            
        }
        return bestNode;
    }

    void MaxRectsBin::pruneFreeList(){
        // Test all newly introduced free rectangles against old free rectangles.
        for(auto freeRectangle : freeRectangles)
            for(size_t j = 0; j < newFreeRectangles.size();)
            {
                if (isContainedIn(newFreeRectangles[j], freeRectangle))
                {
                    newFreeRectangles[j] = newFreeRectangles.back();
                    newFreeRectangles.pop_back();
                }
                else
                {
                    ++j;
                }
            }

        // Merge new and old free rectangles to the group of old free rectangles.
        freeRectangles.insert(freeRectangles.end(), newFreeRectangles.begin(), newFreeRectangles.end());
        newFreeRectangles.clear();
    }
}  // namespace packers
