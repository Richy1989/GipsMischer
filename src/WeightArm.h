#ifndef WEIGHT_ARM_H
#define WEIGHT_ARM_H

class WeightArm {
public:
    WeightArm();

    void setName(const char* outsideName);
    const char* getName() const;

    void setMixtureRatioGramm(int ratio);
    int getMixtureRatioGramm() const;

    void setMixtureRatioMilliLiter(int ratio);
    int getMixtureRatioMilliLiter() const;

    void setWeight(int index, int value);
    int getWeight(int index);

   // const char* getDisplayText() const;

private:
    const char* localName;
    int mixtureRatioGramm;
    int mixtureRatioMilliLiter;
    int weightArray[10];
};

#endif // WEIGHT_ARM_H