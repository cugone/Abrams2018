#pragma once

class Vector2;

class IntVector2 {
public:

    static const IntVector2 ZERO;
    static const IntVector2 ONE;
    static const IntVector2 X_AXIS;
    static const IntVector2 Y_AXIS;
    static const IntVector2 XY_AXIS;
    static const IntVector2 YX_AXIS;

    IntVector2() = default;
    ~IntVector2() = default;

    IntVector2(const IntVector2& rhs) = default;
    IntVector2(IntVector2&& rhs) = default;

    IntVector2(int initialX, int initialY);
    IntVector2(const Vector2& v2);

    IntVector2& operator=(const IntVector2& rhs) = default;
    IntVector2& operator=(IntVector2&& rhs) = default;
    
    bool operator==(const IntVector2& rhs);
    bool operator!=(const IntVector2& rhs);

    void SetXY(int newX, int newY);
    void GetXY(int& out_x, int& out_y);

    int x;
    int y;

protected:
private:

};