// mgnearbz.cpp: 实现贝塞尔曲线段最近点计算函数 mgNearestOnBezier
// Copyright (c) 2004-2012, Zhang Yungui
// License: LGPL, https://github.com/rhcad/touchdraw

#include "mgcurv.h"

#define DEGREE  3           // Cubic Bezier curve
#define W_DEGREE 5          // Degree of eqn to find roots of
#define MAXDEPTH 64         // Maximum depth for recursion

#define SGN(a) (((a)<0) ? -1 : 1)

// ConvertToBezierForm :
//      Given a point and a Bezier curve, generate a 5th-degree
//      Bezier-format equation whose solution finds the point on the
//      curve nearest the user-defined point.
// Parameters :
//      pt: The point to find t for
//      pts: The control points
//      w: Ctl pts of 5th-degree curve, [W_DEGREE+1]
//
static void ConvertToBezierForm(const Point2d& pt, const Point2d* pts, Point2d* w)
{
    int     i, j, k, m, n, ub, lb;
    int     row, column;            // Table indices
    Vector2d    c[DEGREE+1];        // pts(i)'s - pt
    Vector2d    d[DEGREE];          // pts(i+1) - pts(i)
    double  cdTable[3][4];          // Dot product of c, d
    const double z[3][4] = {        // Precomputed "z" for cubics
        {1.0, 0.6, 0.3, 0.1},
        {0.4, 0.6, 0.6, 0.4},
        {0.1, 0.3, 0.6, 1.0},
    };


    // Determine the c's -- these are vectors created by subtracting
    // point pt from each of the control points
    for (i = 0; i <= DEGREE; i++) {
        c[i] = pts[i] - pt;
    }
    // Determine the d's -- these are vectors created by subtracting
    // each control point from the next
    for (i = 0; i <= DEGREE - 1; i++) {
        d[i] = 3.f * (pts[i+1] - pts[i]);
    }

    // Create the c,d table -- this is a table of dot products of the
    // c's and d's
    for (row = 0; row <= DEGREE - 1; row++) {
        for (column = 0; column <= DEGREE; column++) {
            cdTable[row][column] = d[row].dotProduct(c[column]);
        }
    }

    // Now, apply the z's to the dot products, on the skew diagonal
    // Also, set up the x-values, making these "points"
    for (i = 0; i <= W_DEGREE; i++) {
        w[i].y = 0.f;
        w[i].x = static_cast<float>(i) / W_DEGREE;
    }

    n = DEGREE;
    m = DEGREE-1;
    for (k = 0; k <= n + m; k++) {
        lb = mgMax(0, k - m);
        ub = mgMin(k, n);
        for (i = lb; i <= ub; i++) {
            j = k - i;
            w[i+j].y += (float)(cdTable[j][i] * z[j][i]);
        }
    }
}

// CrossingCount :
//      getCount the number of times a Bezier control polygon
//      crosses the 0-axis. This number is >= the number of roots.
// Parameters :
//      pts: Control pts
//      degree: Degree of bezier curve
//
static int CrossingCount(const Point2d* pts, int degree)
{
    int     i;
    int     n_crossings = 0;    // Number of zero-crossings
    int     sign, old_sign;     // Sign of coefficients

    sign = old_sign = SGN(pts[0].y);
    for (i = 1; i <= degree; i++) {
        sign = SGN(pts[i].y);
        if (sign != old_sign) n_crossings++;
        old_sign = sign;
    }
    return n_crossings;
}

// ControlPolygonFlatEnough :
//      Check if the control polygon of a Bezier curve is flat enough
//      for recursive subdivision to bottom out.
// Parameters :
//      pts: Control pts
//      degree: Degree of bezier curve
//
static int ControlPolygonFlatEnough(const Point2d* pts, int degree)
{
    int     i;                      // Index variable
    double  distance[W_DEGREE+1];   // Distances from pts to line
    double  max_distance_above;     // maximum of these
    double  max_distance_below;
    double  error;                  // Precision of root
    double  intercept_1,
        intercept_2,
        left_intercept,
        right_intercept;
    double  a, b, c;    // Coefficients of implicit eqn for line from pts[0]-pts[deg]

    // Find the  perpendicular distance
    // from each interior control point to
    // line connecting pts[0] and pts[degree]
    {
        double  abSquared;

        // Derive the implicit equation for line connecting first *'
        // and last control points
        a = pts[0].y - pts[degree].y;
        b = pts[degree].x - pts[0].x;
        c = pts[0].x * pts[degree].y - pts[degree].x * pts[0].y;

        abSquared = (a * a) + (b * b);

        for (i = 1; i < degree; i++)
        {
            // Compute distance from each of the points to that line
            distance[i] = a * pts[i].x + b * pts[i].y + c;
            if (distance[i] > 0.0) {
                distance[i] = (distance[i] * distance[i]) / abSquared;
            }
            if (distance[i] < 0.0) {
                distance[i] = -((distance[i] * distance[i]) / abSquared);
            }
        }
    }


    // Find the largest distance
    max_distance_above = 0.0;
    max_distance_below = 0.0;
    for (i = 1; i < degree; i++)
    {
        if (distance[i] < 0.0) {
            max_distance_below = mgMin(max_distance_below, distance[i]);
        };
        if (distance[i] > 0.0) {
            max_distance_above = mgMax(max_distance_above, distance[i]);
        }
    }

    {
        double  det, dInv;
        double  a1, b1, c1, a2, b2, c2;

        // Implicit equation for zero line
        a1 = 0.0;
        b1 = 1.0;
        c1 = 0.0;

        // Implicit equation for "above" line
        a2 = a;
        b2 = b;
        c2 = c + max_distance_above;

        det = a1 * b2 - a2 * b1;
        dInv = 1.0 / det;

        intercept_1 = (b1 * c2 - b2 * c1) * dInv;

        // Implicit equation for "below" line
        a2 = a;
        b2 = b;
        c2 = c + max_distance_below;

        det = a1 * b2 - a2 * b1;
        dInv = 1.0 / det;

        intercept_2 = (b1 * c2 - b2 * c1) * dInv;
    }

    // Compute intercepts of bounding box
    left_intercept = mgMin(intercept_1, intercept_2);
    right_intercept = mgMax(intercept_1, intercept_2);

    error = 0.5f * (right_intercept-left_intercept);
    if (error < _MGZERO) {
        return 1;
    }
    else {
        return 0;
    }
}

// ComputeXIntercept :
//      Compute intersection of chord from first control point to last
//      with 0-axis.
// NOTE: "T" and "Y" do not have to be computed, and there are many useless
//      operations in the following (e.g. "0.0 - 0.0").
// Parameters :
//      pts: Control points
//      degree: Degree of curve
//
static double ComputeXIntercept(const Point2d* pts, int degree)
{
    double  XLK, YLK, XNM, YNM, XMK, YMK;
    double  det, detInv;
    double  S;
    double  X;

    XLK = 1.0 - 0.0;
    YLK = 0.0 - 0.0;
    XNM = pts[degree].x - pts[0].x;
    YNM = pts[degree].y - pts[0].y;
    XMK = pts[0].x - 0.0;
    YMK = pts[0].y - 0.0;

    det = XNM*YLK - YNM*XLK;
    detInv = 1.0 / det;

    S = (XNM*YMK - YNM*XMK) * detInv;
    // T = (XLK*YMK - YLK*XMK) * detInv;

    X = 0.0 + XLK * S;
    // Y = 0.0 + YLK * S;

    return X;
}

// BezierPoint :
//      Evaluate a Bezier curve at a particular parameter value
//      Fill in control points for resulting sub-curves if "Left" and
//      "Right" are non-null.
// Parameters :
//      pts: Control pts
//      degree: Degree of bezier curve
//      t: Parameter value
//      Left: output left half ctl pts
//      Right: output right half ctl pts
//
static Point2d BezierPoint(const Point2d* pts, int degree, double t, 
                           Point2d* Left, Point2d* Right)
{
    int     i, j;       // Index variables
    Point2d Vtemp[W_DEGREE+1][W_DEGREE+1];


    // Copy control points
    for (j =0; j <= degree; j++) {
        Vtemp[0][j] = pts[j];
    }

    // Triangle computation
    for (i = 1; i <= degree; i++) {
        for (j =0 ; j <= degree - i; j++) {
            Vtemp[i][j].x = (float)(
                (1.0 - t) * Vtemp[i-1][j].x + t * Vtemp[i-1][j+1].x);
            Vtemp[i][j].y = (float)(
                (1.0 - t) * Vtemp[i-1][j].y + t * Vtemp[i-1][j+1].y);
        }
    }

    if (Left != NULL) {
        for (j = 0; j <= degree; j++) {
            Left[j]  = Vtemp[j][0];
        }
    }
    if (Right != NULL) {
        for (j = 0; j <= degree; j++) {
            Right[j] = Vtemp[degree-j][j];
        }
    }

    return (Vtemp[degree][0]);
}

// FindRoots :
//      Given a 5th-degree equation in Bernstein-Bezier form, find
//      all of the roots in the interval [0, 1].  Return the number
//      of roots found.
// Parameters :
//      w: The control points
//      degree: The degree of the polynomial
//      t: output candidate t-values
//      depth: The depth of the recursion
//
static int FindRoots(const Point2d* w, int degree, double* t, int depth)
{
    int i;
    Point2d Left[W_DEGREE+1];   // New left and right
    Point2d Right[W_DEGREE+1];  // control polygons
    int     left_count;         // Solution count from children
    int     right_count;
    double  left_t[W_DEGREE+1]; // Solutions from kids
    double  right_t[W_DEGREE+1];

    switch (CrossingCount(w, degree))
    {
    case 0 :    // No solutions here
        return 0;

    case 1 :    // Unique solution
        // Stop recursion when the tree is deep enough
        // if deep enough, return 1 solution at midpoint
        if (depth >= MAXDEPTH)
        {
            t[0] = (w[0].x + w[W_DEGREE].x) / 2.0;
            return 1;
        }
        if (ControlPolygonFlatEnough(w, degree)) {
            t[0] = ComputeXIntercept(w, degree);
            return 1;
        }
        break;
    }

    // Otherwise, solve recursively after subdividing control polygon
    BezierPoint(w, degree, 0.5f, Left, Right);
    left_count  = FindRoots(Left,  degree, left_t, depth+1);
    right_count = FindRoots(Right, degree, right_t, depth+1);

    // Gather solutions together
    for (i = 0; i < left_count; i++) {
        t[i] = left_t[i];
    }
    for (i = 0; i < right_count; i++) {
        t[i+left_count] = right_t[i];
    }

    // Send back total number of solutions
    return (left_count+right_count);
}

// 计算一点到三次贝塞尔曲线段上的最近点
// Parameters :
//      pt: The user-supplied point
//      pts: Control points of cubic Bezier
//      ptNear: output the point on the curve at that parameter value
//
GEOMAPI void mgNearestOnBezier(
    const Point2d& pt, const Point2d* pts, Point2d& ptNear)
{
    Point2d w[W_DEGREE+1];          // Ctl pts for 5th-degree eqn
    double  t_candidate[W_DEGREE];  // Possible roots
    int     n_solutions;            // Number of roots found
    double  t;                      // Parameter value of closest pt

    // Convert problem to 5th-degree Bezier form
    ConvertToBezierForm(pt, pts, w);

    // Find all possible roots of 5th-degree equation
    n_solutions = FindRoots(w, W_DEGREE, t_candidate, 0);

    // Compare distances of pt to all candidates, and to t=0, and t=1
    {
        double  dist, new_dist;
        Point2d p;
        int     i;


        // Check distance to beginning of curve, where t = 0
        dist = (pt - pts[0]).lengthSqrd();
        t = 0.0;

        // Find distances for candidate points
        for (i = 0; i < n_solutions; i++) {
            p = BezierPoint(pts, DEGREE, t_candidate[i],
                (Point2d *)NULL, (Point2d *)NULL);
            new_dist = (pt - p).lengthSqrd();
            if (new_dist < dist) {
                dist = new_dist;
                t = t_candidate[i];
            }
        }

        // Finally, look at distance to end point, where t = 1.0
        new_dist = (pt - pts[DEGREE]).lengthSqrd();
        if (new_dist < dist) {
            dist = new_dist;
            t = 1.0;
        }
    }

    // Return the point on the curve at parameter value t
    // printf("t : %4.12f\n", t);
    ptNear = (BezierPoint(pts, DEGREE, t, (Point2d *)NULL, (Point2d *)NULL));
}
