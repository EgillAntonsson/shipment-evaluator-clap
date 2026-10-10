#pragma once

#include <atomic>
#include <cstdint>

class ShipmentEvaluatorGui {
public:
    explicit ShipmentEvaluatorGui(const std::atomic<double> *meter);
    ~ShipmentEvaluatorGui();

    bool create();
    void destroy();
    bool get_size(uint32_t *width, uint32_t *height) const;
    bool set_size(uint32_t width, uint32_t height);
    bool set_parent(void *parent_view);
    bool show();
    bool hide();

private:
    const std::atomic<double> *meter_;
    void *view_ = nullptr;
};
