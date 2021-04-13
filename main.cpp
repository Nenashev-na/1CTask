#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <set>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define DEPTH 40
#define LINE 6

/*
 * Программа считывает имя картинки из stdin и выдает ответ в stdout.
 * Идея в том, чтобы запустить bfs с определенной глубиной и вычислить узел.
 * Идеально было бы, если после работы bfs определить пересечения с окресностью,
 * отстающей от исходной точки на глубину bfs, а затем посмотреть сколько таких точек.
 * Если таких точек > 2 и хотя бы три лежат на одной прямой с той, от которой запускались - то это пересечение.
 * Но за недостатком времени я пошел простым путем и определяю конечное число точек пересечения как размер вектора,
 * который получается после остановки bfs, деленый на характерную ширину линий.
 * Если таких точек > 2, то считаем что это или вершина или пересечение.
 * Для считывания картинки использовал OpenCV.
*/

class Image {
public:
    explicit Image(const cv::Mat& frame) {
        height = frame.size().height;
        width = frame.size().width;
        this->data = std::vector<std::vector<int16_t>>(height,std::vector<int16_t>(width));
        this->used = std::vector<std::vector<int16_t>>(height,std::vector<int16_t>(width, 0));
        for (size_t i = 0; i < height; i++) {
            for (size_t j = 0; j < width; j++) {
                auto color = (uint32_t)frame.at<u_char>(i, j);
                data[i][j] = color == 255 ? 0 : 1; // 0 - WHITE, 1 - BLACK;
            }
        }
    }
    int FindCrossingCount() {
        int count = 0;
        for (size_t i = 0; i < height; i++) {
            for (size_t j = 0; j < width; j++) {
                used.clear();
                count += BreadthFirstSearch({i, j, data[i][j]});
            }
        }
        return count;
    }
private:
    struct Point {
        Point(size_t _x, size_t _y, int16_t _color) : x(_x), y(_y), color(_color) {};
        size_t x;
        size_t y;
        int16_t color;
    };
    bool BreadthFirstSearch(const Point& p, size_t depth=DEPTH) {
        if (p.color == 0 || used[p.x][p.y])
            return false;
        std::queue<Point> q;
        q.push(p);
        size_t depth_conter = 0;
        while (!q.empty()) {
            if (depth_conter > depth) {
                return CheckResult(q);
            }
            Point u = q.front();
            q.pop();
            used[u.x][u.y] = 1;
            if (u.x >= 1 && u.y >= 1 && !used[u.x - 1][u.y - 1] && data[u.x - 1][u.y - 1] == 1) {
                used[u.x - 1][u.y - 1] = 1;
                q.push({u.x - 1, u.y - 1, 1});
            }
            if (u.x + 1 < height && u.y + 1 < width && !used[u.x + 1][u.y + 1] && data[u.x + 1][u.y + 1] == 1) {
                used[u.x + 1][u.y + 1] = 1;
                q.push({u.x + 1, u.y + 1, 1});
            }
            if (u.x >= 1 && u.y + 1 < width && !used[u.x - 1][u.y + 1] && data[u.x - 1][u.y + 1] == 1) {
                used[u.x - 1][u.y + 1] = 1;
                q.push({u.x - 1, u.y + 1, 1});
            }
            if (u.x + 1 < height && u.y >= 1 && !used[u.x + 1][u.y - 1] && data[u.x + 1][u.y - 1] == 1) {
                used[u.x + 1][u.y - 1] = 1;
                q.push({u.x + 1, u.y - 1, 1});
            }
            if (u.x >= 1 && !used[u.x - 1][u.y] && data[u.x - 1][u.y] == 1) {
                used[u.x - 1][u.y] = 1;
                q.push({u.x - 1, u.y, 1});
            }
            if (u.x + 1 < height && !used[u.x + 1][u.y] && data[u.x + 1][u.y] == 1) {
                used[u.x + 1][u.y] = 1;
                q.push({u.x + 1, u.y, 1});
            }
            if (u.y >= 1 && !used[u.x][u.y - 1] && data[u.x][u.y - 1] == 1) {
                used[u.x][u.y - 1] = 1;
                q.push({u.x, u.y - 1, 1});
            }
            if (u.y + 1 < width && !used[u.x][u.y + 1] && data[u.x][u.y + 1] == 1) {
                used[u.x][u.y + 1] = 1;
                q.push({u.x, u.y + 1, 1});
            }
            depth_conter++;
        }
        return false;
    }
    static bool CheckResult(std::queue<Point>& q) {
        std::vector<Point> st;
        while (!q.empty()) {
            st.push_back(q.front());
            q.pop();
        }
        if (st.size() / LINE > 2) {
            return true;
        }
        return false;
    }
    std::vector<std::vector<int16_t>> used;
    std::vector<std::vector<int16_t>> data;
    size_t height;
    size_t width;
};


int main(int argc, char** argv) {
    cv::Mat frame;
    std::ofstream fout("out.txt");
    std::string filename;
    std::cout << "Enter image name: ";
    std::cin >> filename;
    std::cout << std::endl;
    frame = cv::imread(filename, cv::IMREAD_GRAYSCALE);

    if(!frame.data) {
        std::cout << "Could not open or find the frame" << std::endl;
        return -1;
    }

    Image image(frame);
    std::cout << image.FindCrossingCount() << std::endl;
    /* Вывод картинки
    cv::namedWindow("Window", cv::WINDOW_AUTOSIZE);
    cv::imshow("Window", frame);
    cv::waitKey(0);
     */
    return 0;
}
