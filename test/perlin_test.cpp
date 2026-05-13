#include <SFML/Graphics.hpp>
#include "../ind/data_maker.hpp"
#include <iostream>

int main()
{
    freopen("perlin_noise.txt", "w", stdout);

    // 创建柏林噪声对象
    double width = 100;
    double height = 100;
    double blockSize = 2;
    std::mt19937 rng{std::random_device{}()};
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::cin >> width >> height >> blockSize;
    
    PerlinNoise perlin(ceil(width / blockSize), ceil(height / blockSize), blockSize);
    for (double x = 0.0; x < width; x += 0.1) {
        for (double y = 0.0; y < height; y += 0.1) {
            double noise = perlin.noise(x, y);
            std::cout << noise << ' ';
        }
        std::cout << std::endl;
    }


    return 0;
}