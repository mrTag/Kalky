#ifndef KALKY_SUMVALUE_H
#define KALKY_SUMVALUE_H

#include "BaseValue.h"

namespace kalky {

    class SumValue : public BaseValue {
    public:

        void AddValueToSum(BaseValue* value) {
            _sumValues.push_back(value);
            _autoDestroyConnections.emplace_back(
                value->OnValueDeleted.connect([this](kalky::BaseValue *value) {
                    auto valIter = _sumValues.begin();
                    while(valIter != _sumValues.end()) {
                        if(*valIter == value) {
                            _sumValues.erase(valIter);
                            SetValueDirty();
                            return;
                        }
                        ++valIter;
                    }
                }));
            // when anything changes in the summed values, we are dirty as well...
            _autoDestroyConnections.emplace_back(
                value->OnValueChanged.connect([this](kalky::BaseValue *) {
                    SetValueDirty();
                }));
            SetValueDirty();
        }

    protected:
        std::vector<BaseValue*> _sumValues;

        void RecalculateCachedValue() override {
            float sum = 0;
            for(auto& value : _sumValues) {
                sum += value->GetValue();
            }
            CacheValue(sum);
        }
    private:
        std::vector<fteng::connection> _autoDestroyConnections;
    };

}

#endif //KALKY_SUMVALUE_H
