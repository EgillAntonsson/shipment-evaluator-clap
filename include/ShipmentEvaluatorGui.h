#pragma once

#include <atomic>
#include <cstdint>

class ShipmentEvaluatorGui {
public:
    explicit ShipmentEvaluatorGui(const std::atomic<double> *meter);
    ~ShipmentEvaluatorGui();

    bool create(bool is_floating);
    void destroy();
    bool get_size(uint32_t *width, uint32_t *height) const;
    bool set_size(uint32_t width, uint32_t height);
    bool set_parent(void *parent_view);
    bool set_transient(void *parent_view);
    void suggest_title(const char *title);
    bool show();
    bool hide();

private:
    const std::atomic<double> *meter_;
    void *view_ = nullptr;
    void *window_ = nullptr;
};
