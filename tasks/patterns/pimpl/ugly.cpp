#include "bad.h"
#include "ugly.h"
#include <memory>
#include <vector>

struct SplineImpl {
    std::vector<double> x, y, y2;
};

Spline::Spline(const std::vector<double>& x, const std::vector<double>& y, double a, double b) {
    impl_ = std::make_shared<SplineImpl>();
    impl_->x = x;
    impl_->y = y;
    impl_->y2 = std::vector<double>(x.size() + 2);
    mySplineSnd(impl_->x.data(), impl_->y.data(), impl_->x.size(), a, b, impl_->y2.data());
}

double Spline::Interpolate(double x) {
    double ans = 0;
    mySplintCube(impl_->x.data(), impl_->y.data(), impl_->y2.data(), impl_->x.size(), x, &ans);
    return ans;
}
