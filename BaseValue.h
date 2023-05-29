#ifndef KALKY_BASEVALUE_H
#define KALKY_BASEVALUE_H

#include <signals.hpp>

namespace kalky {

    class BaseValue {
    public:
        BaseValue & operator=(const BaseValue&) = delete;
        BaseValue(BaseValue const &) = delete;
        BaseValue() = default;

        virtual ~BaseValue() {
            OnValueDeleted(this);
        }

        fteng::signal<void(BaseValue* Value)> OnValueChanged;
        fteng::signal<void(BaseValue* Value)> OnValueDeleted;

        float GetValue() {
            if(_isDirty)
                RecalculateCachedValue();
            return _cachedValue;
        }
    protected:
        bool _isDirty = true;
        float _cachedValue;

        virtual void RecalculateCachedValue() = 0;

        void SetValueDirty() {
            _isDirty = true;
            OnValueChanged(this);
        }

        void CacheValue(float cachedValue) {
            _isDirty = false;
            _cachedValue = cachedValue;
        }

    };

}

#endif //KALKY_BASEVALUE_H