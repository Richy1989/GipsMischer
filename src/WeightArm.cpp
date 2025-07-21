#include "WeightArm.h"

// WeightArm::WeightArm() : name("Default"), mixtureRatioGramm(0), mixtureRatioMilliLiter(0) {
 //   for (int i = 0; i < 10; ++i) {
//        weightArray[i] = 0;
 //   }


void WeightArm::setName(const char* outsideName) {
    this->localName = outsideName;
   // Save();
}

const char* WeightArm::getName() const {
    return localName;
}

void WeightArm::setMixtureRatioGramm(int ratio) {
    mixtureRatioGramm = ratio;
}

int WeightArm::getMixtureRatioGramm() const {
    return mixtureRatioGramm;
}

void WeightArm::setMixtureRatioMilliLiter(int ratio) {
    mixtureRatioMilliLiter = ratio;
}

int WeightArm::getMixtureRatioMilliLiter() const {
    return mixtureRatioMilliLiter;
}


// void WeightArm::getDisplayText() const {
 //   static char displayText[32];
    //snprintf(displayText, sizeof(displayText), "Name: %s, Ratio: %d/%d", localName, mixtureRatioGramm, mixtureRatioMilliLiter);
 //   return displayText;




