
#include <mantis/lib/classes/time_tag.h>

mantis::time_tag::time_tag(int64_t full_secs, double frac_secs) : full_seconds(full_secs), frac_secs(frac_secs) {}

mantis::time_tag::time_tag(int64_t full_secs) : full_seconds(full_secs), frac_secs(0) {}

int64_t mantis::time_tag::get_full_secs() {
    return this->full_seconds;
}

double mantis::time_tag::get_frac_secs() {
    return this->frac_secs;
}

mantis::time_tag mantis::time_tag::operator+(time_tag other) const {
    auto full = this->full_seconds + other.full_seconds;

    auto frac_and_remainder = this->frac_secs + other.frac_secs;

    if(frac_and_remainder >= 1.0) {
        frac_and_remainder -= 1.0;
        full += 1;
    }

    return {full, frac_and_remainder};
}

mantis::time_tag mantis::time_tag::operator-(time_tag other) const {
    auto frac = this->frac_secs - other.frac_secs;
    int64_t full = this->full_seconds - other.full_seconds;

    if (frac < 0) {
        frac = 1 + frac; // frac is negative 
        full--;
    }

    return {full, frac};
}

mantis::time_tag &mantis::time_tag::operator+=(time_tag other) {
    auto result = *this + other;

    this->frac_secs = result.frac_secs;
    this->full_seconds = result.full_seconds;

    return *this;
}

mantis::time_tag &mantis::time_tag::operator-=(time_tag other) {
    auto result = *this - other;

    this->frac_secs = result.frac_secs;
    this->full_seconds = result.full_seconds;

    return *this;
}