#include <stdio.h>
#include <GL/glut.h>
#include <string.h>
#include <curl/curl.h>
#include <stdbool.h>
#include <math.h>

int windowWidth = 800;
int windowHeight = 800;
int gridSize = 50;

typedef struct {
    int x;
    int y;
} Point;

typedef struct{
	Point points[10];
}allPoints;

Point points[100];
Point mypoint[50];
allPoints allpoints[4];
int mypointCount = 0;
int pointCount = 0;
bool isShapeClosed = false;

void ParseCoordinates(const char *data, int startLine) {
    int x, y;
    const char *ptr = data;
    int currentLine = 1;
    char lineBuffer[100];

    while (*ptr) {
        if (currentLine == startLine) {
            break;
        } else {
            ptr = strchr(ptr, '\n');
            if (ptr) {
                ptr++;
                currentLine++;
            } else {
                break;
            }
        }
    }

    if (*ptr) {
        int i = 0;
        while (*ptr && *ptr != '\n' && i < sizeof(lineBuffer) - 1) {
            if (*ptr == '(') {
                ptr++;
                int x, y;
                if (sscanf(ptr, "%d,%d", &x, &y) == 2) {
						x *= 10;
						y *= 10;
                    if (pointCount > 0 && x == points[0].x && y == points[0].y) {
                        isShapeClosed = true;
                    }
                    points[pointCount].x = x;
                    points[pointCount].y = y;
                    pointCount++;
                }
                while (*ptr && *ptr != ')') {
                    ptr++;
                }
                if (*ptr == ')') {
                    ptr++;
                }
            } else {
                lineBuffer[i++] = *ptr;
                ptr++;
            }
        }
        lineBuffer[i] = '\0';
    }
}

float CalculateArea(Point* mypoint, int pointCount) {
    float area = 0.0;
    int i;

    for (i = 0; i < pointCount; i++) {
        int x1 = mypoint[i].x / 10;
        int y1 = mypoint[i].y / 10;

        int x2 = mypoint[(i + 1) % pointCount].x / 10;
        int y2 = mypoint[(i + 1) % pointCount].y / 10;

        area += (x1 * y2 - x2 * y1);
    }

    area = fabs(area) / 2.0;  
    return area;
}



int findCount(Point* wow){
	int a = 0;
	int length = 0;
	while((wow[a].x != 0) && (wow[a].y != 0)){
		a++;
		length++;
	}
	return length;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 1.0);

    for (int x = 0; x < windowWidth; x += gridSize) {
        for (int y = 0; y < windowHeight; y += gridSize) {
            glBegin(GL_LINE_LOOP);
            glVertex2i(x, y);
            glVertex2i(x + gridSize, y);
            glVertex2i(x + gridSize, y + gridSize);
            glVertex2i(x, y + gridSize);
            glEnd();
        }
    }

    glColor3f(0.0f, 1.0f, 0.0f);

    glBegin(GL_LINE_STRIP);
    int checkX;
    int checkY;
    int m;
    for(int i = 0, m = 0, l = 0; i < pointCount; i++){
    	if(i == 0){
    		checkX = points[i].x;
    		checkY = points[i].y;
    		allpoints[m].points[l].x = points[i].x;
			allpoints[m].points[l].y = points[i].y;
			l++;
		}
		else if((points[i].x == checkX )&& (points[i].y == checkY)){
			allpoints[m].points[l].x = points[i].x;
			allpoints[m].points[l].y = points[i].y;
			m++;
			l=0;
			checkX = points[i + 1].x;
			checkY = points[i + 1].y;
			allpoints[m].points[l].x = checkX; 
			allpoints[m].points[l].y = checkY;
			i++;
			l++;
		}
		else{
			allpoints[m].points[l].x = points[i].x;
			allpoints[m].points[l].y = points[i].y;
			l++;
		}
	}
	for(int x = 0 ; x < 4 ; x++){
		glBegin(GL_LINE_STRIP);
		for(int p = 0; (allpoints[x].points[p].x != 0) && (allpoints[x].points[p].y != 0); p++){
			glVertex2i(allpoints[x].points[p].x, windowHeight - allpoints[x].points[p].y);
		}
		glEnd();
	}
	float toplam = 0;
	for(int s = 0; s < allpoints[s].points[0].x != 0; s++){
	
		    float area = CalculateArea(allpoints[s].points, findCount(allpoints[s].points));
    		printf("%d. Alan: %.1f\n",s + 1 ,area);
			toplam += area;
	}
	printf("Toplam Alan : %.1f",toplam);
    glFlush();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
}

size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    char *data = (char *)contents;

    int startLine;
    printf("Hangi satirdaki koordinatlari kullanmak istersiniz? Satir numarasi girin: ");
    scanf("%d", &startLine);

    ParseCoordinates(data, startLine);

    glutPostRedisplay();

    return size * nmemb;
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Kare Grid");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://zplusorg.com/prolab.txt");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    glutMainLoop();

    return 0;
}

