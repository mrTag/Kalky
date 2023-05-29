#ifndef KALKY_FLATVALUE_H
#define KALKY_FLATVALUE_H

#include "BaseValue.h"

namespace kalky {

    class FlatValue : public BaseValue {
    public:
        void SetValue(float newValue) {
            if(newValue == _cachedValue)
                return;
            CacheValue(newValue);
            OnValueChanged(this);
        }
    private:
        void RecalculateCachedValue() override {
            // nothing to do here...
        }
    };
}

#endif //KALKY_FLATVALUE_H
