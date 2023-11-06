#ifndef INTERVAL_H
#define INTERVAL_H

class Interval {
public:
    double min, max;

    Interval() : min(+infinity), max(-infinity) {}  // default interval is empty
    Interval(double _min, double _max) : min(_min), max(_max) {}
    Interval(const Interval& a, const Interval& b)
        : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    bool contains(double x) const { return min <= x && x <= max; }

    bool surrounds(double x) const { return min < x && x < max; }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    double size() const { return max - min; }

    Interval expand(const double delta) const {
        double padding = delta / 2;
        return Interval(min - padding, max + padding);
    }

    static const Interval empty, universe;
};

const static Interval empty(+infinity, -infinity);
const static Interval universe(-infinity, +infinity);

Interval operator+(const Interval& ival, double offset) {
    return Interval(ival.min + offset, ival.max + offset);
}

Interval operator+(double offset, const Interval& ival) { return ival + offset; }

#endif  // INTERVAL_H