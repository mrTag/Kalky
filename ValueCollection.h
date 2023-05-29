#ifndef KALKY_VALUECOLLECTION_H
#define KALKY_VALUECOLLECTION_H

#include "BaseValue.h"
#include <unordered_map>
#include <string>
#include <signals.hpp>

namespace kalky {

    class ValueCollection {
    public:
        ~ValueCollection() {
            OnValueCollectionDeleted(this);
        }

        fteng::signal<void(const std::string& name, BaseValue* value)> OnValueChanged;
        fteng::signal<void(ValueCollection* collection)> OnValueCollectionDeleted;

        void AddNamedValue(const std::string& name, BaseValue* value) {
            _namedValues[name] = value;

            // automatically remove values, when they get deleted...
            _autoDestroyConnections.emplace_back(
                value->OnValueDeleted.connect([this,name](kalky::BaseValue *) {
                    auto namedValueIter = _namedValues.find(name);
                    if(namedValueIter != _namedValues.end())
                        _namedValues.erase(namedValueIter);
                }));
            // re-trigger the OnValueChanged Signal with the name
            _autoDestroyConnections.emplace_back(
                value->OnValueChanged.connect([this,name](kalky::BaseValue *value) {
                    OnValueChanged(name, value);
                }));
        }

        template<class StringType>
        auto find(const StringType& name) {
            return _namedValues.find(name);
        }
        auto begin() {
            return _namedValues.begin();
        }
        auto end() {
            return _namedValues.end();
        }
    private:
        std::unordered_map<std::string, BaseValue*> _namedValues;
        std::vector<fteng::connection> _autoDestroyConnections;
    };

}

#endif //KALKY_VALUECOLLECTION_H
