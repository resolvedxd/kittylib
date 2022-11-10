#ifndef KITTY_COLOR
#define KITTY_COLOR
#include <cstdlib>
#include <cstdio>
#include <cstdint>
#include <math.h>

struct hsl_t {
    float h, s, l;
    int a;
};

struct hsv_t {
    float h, s, v;
    int a;
};

struct color_t {
    unsigned char a, r, g, b;
    color_t() = default;
    color_t(color_t col, int alpha) {
        this->r = col.r;
        this->g = col.g;
        this->b = col.b;
        this->a = alpha;
    }
    color_t(const int r, const int g, const int b, const int a = 255) {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;
    }
    explicit color_t(const uint32_t color) {
        this->a = (color >> 24) & 0xff;
        this->r = (color >> 16) & 0xff;
        this->g = (color >> 8) & 0xff;
        this->b = (color & 0xff);
    }
    static color_t from_uint(const uint32_t uint) {
        return color_t(uint);
    }

    operator unsigned int() const {
        return (a << 24) | (r << 16) | (g << 8) | b;
    }
    
    uint32_t to_uint() {
        uint32_t uint = 0;
        uint |= (a & 255) << 24;
        uint |= (r & 255) << 16;
        uint |= (g & 255) << 8;
        uint |= (b & 255);
        return uint;
    }

    static color_t from_hex_str(const char* str) {
        uint32_t uint = strtoul(str, NULL, 16);
        return color_t(uint | 0xff000000);
    }

    const char* to_hex_str() {
        static char str[7];
        sprintf(str, "%06x", to_uint() & 0xffffff);
        return str;
    }

    bool operator==(const color_t& r) {
        return this->r == r.r &&
            this->g == r.g &&
            this->b == r.b &&
            this->a == r.a;
    }

    bool operator!=(const color_t& r) {
        return this->r != r.r ||
            this->g != r.g ||
            this->b != r.b ||
            this->a != r.a;
    }

    color_t lerp(color_t to_color, float fraction) {
        return color_t((to_color.r - this->r) * fraction + this->r,
            (to_color.g - this->g) * fraction + this->g,
            (to_color.b - this->b) * fraction + this->b);
    }

    unsigned char luma() {
        return 0.2126 * this->r + 0.7152 * this->g + 0.0722 * this->b;
    }

    static color_t black(const int  a = 255) { return { 0, 0, 0, a }; }
    static color_t white(const int  a = 255) { return { 255, 255, 255, a }; }
    static color_t red(const int    a = 255) { return { 255, 0, 0, a }; }
    static color_t yellow(const int a = 255) { return { 255, 255, 0, a }; }
    static color_t green(const int  a = 255) { return { 0, 255, 0, a }; }
    static color_t cyan(const int   a = 255) { return { 0, 255, 255, a }; }
    static color_t blue(const int   a = 255) { return { 0, 0, 255, a }; }

    // RGB to HSL
    hsl_t to_hsl() {
        float r_ = r / 255.0;
        float g_ = g / 255.0;
        float b_ = b / 255.0;
        float h, s, l;

        float min = fmin(r_, fmin(g_, b_));
        float max = fmax(r_, fmax(g_, b_));
        float delta = max - min;

        l = (max + min) / 2;
        if (delta == 0)
        {
            h = s = 0;
        }
        else {
            if (l < 0.5) {
                s = delta / (max + min) * 100;
            }
            else {
                s = delta / (1 - abs(2 * l - 1)) * 100;
            }

            if (r_ == max) {
                h = (g_ - b_) / delta;
            }
            else if (g_ == max) {
                h = (b_ - r_) / delta + 2;
            }
            else if (b_ == max) {
                h = (r_ - g_) / delta + 4;
            }
            h = fmod(60 * h + 360, 360);
        }
        l *= 100;

        return hsl_t{ h, s, l, a };
    }

    // HSL to RGB
    static color_t from_hsl(float h, float s, float l, int alpha = 255) {
        s /= 100;
        l /= 100;

        float c = (1 - abs(2 * l - 1)) * s,
            x = c * (1 - abs(fmod((h / 60), 2) - 1)),
            m = l - c / 2,
            r = 0,
            g = 0,
            b = 0;

        if (0 <= h && h < 60) {
            r = c; g = x; b = 0;
        }
        else if (60 <= h && h < 120) {
            r = x; g = c; b = 0;
        }
        else if (120 <= h && h < 180) {
            r = 0; g = c; b = x;
        }
        else if (180 <= h && h < 240) {
            r = 0; g = x; b = c;
        }
        else if (240 <= h && h < 300) {
            r = x; g = 0; b = c;
        }
        else if (300 <= h && h < 361) {
            r = c; g = 0; b = x;
        }

        return color_t(round((r + m) * 255), round((g + m) * 255), round((b + m) * 255), alpha);
    }

    hsv_t to_hsv() {
        float r_ = r / 255.0f;
        float g_ = g / 255.0f;
        float b_ = b / 255.0f;

        float h, s, v; // h:0-360.0, s:0.0-1.0, v:0.0-1.0

        float max = fmaxf(r_, fmaxf(g_, b_));
        float min = fminf(r_, fminf(g_, b_));

        v = max;

        if (max == 0.0f) {
            s = 0;
            h = 0;
        }
        else if (max - min == 0.0f) {
            s = 0;
            h = 0;
        }
        else {
            s = (max - min) / max;

            if (max == r_) {
                h = 60 * ((g_ - b_) / (max - min)) + 0;
            }
            else if (max == g_) {
                h = 60 * ((b_ - r_) / (max - min)) + 120;
            }
            else {
                h = 60 * ((r_ - g_) / (max - min)) + 240;
            }
        }

        if (h < 0) h += 360.0f;

        hsv_t hsv;
        hsv.h = h;
        hsv.s = s * 100;
        hsv.v = v * 100;

        return hsv;
    }

    static color_t from_hsv(const float& src_h, const float& src_s, const float& src_v, int alpha = 255) {
        float h = src_h; // 0-360
        float s = src_s / 100.0f; // 0.0-1.0
        float v = src_v / 100.0f; // 0.0-1.0

        float r, g, b; // 0.0-1.0

        int   hi = (int)(h / 60.001f) % 6;
        float f = (h / 60.001f) - hi;
        float p = v * (1.0f - s);
        float q = v * (1.0f - s * f);
        float t = v * (1.0f - s * (1.0f - f));

        switch (hi) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
        }

        color_t rgb;
        rgb.r = r * 255;
        rgb.g = g * 255;
        rgb.b = b * 255;
        rgb.a = alpha;

        return rgb;
    }
};
#endif
