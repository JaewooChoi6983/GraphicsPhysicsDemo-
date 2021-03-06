//Name: Hyosang Jung, Jaewoo Choi
//Assignment name: Class project
//Course name: CS250
//Term & Year : 2022&Spring
#include"Engine.h"
#include"tests/TestTriangle.h"
#include"tests/Toon_Fog.h"
#include"tests/ValueNoise.h"
#include"tests/GradientNoise.h"
#include"tests/Curve.h"
#include"tests/geometry_test.h"
#include"tests/tessellation.h"
#include"tests/Shadow.h"
#include"tests/PointShadow.h"

int main() 
{
    Engine engine;
    TriangleTest* triangle = new TriangleTest();
    Toon_Fog* toon_fog = new Toon_Fog();
    Noise* value_noise = new Noise();
    Gradient_Noise* gradient_noise = new Gradient_Noise();
    CurveTest* curve_test = new CurveTest();
    Geometry_test* geometry_test = new Geometry_test();
    Tessellation_test* tessellation_test = new Tessellation_test();
    Shadow_test* shadow_test = new Shadow_test();
    PointShadow* pointshadow_test = new PointShadow();


   engine.Add(triangle);
   engine.Add(toon_fog);
   engine.Add(value_noise);
   engine.Add(gradient_noise);
   engine.Add(curve_test);
   engine.Add(geometry_test);
   engine.Add(tessellation_test);
   engine.Add(shadow_test);
   engine.Add(pointshadow_test);
   engine.init();

    while (!engine.ShouldClose()) 
    {
        engine.Update();
        engine.Draw();
    }

    _CrtDumpMemoryLeaks();
}

