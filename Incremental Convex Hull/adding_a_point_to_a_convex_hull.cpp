#include<windows.h>
#include<GL/Glut.h> //includes the opengl, glu, and glut header files
#include<stdlib.h> //includes the standard library header file
#include<iostream>
#include<fstream>
#include<vector>

using namespace std;

/*
 * Program that gets as input a convex hull ( points given are in counter-clockwise order)
 * and by clicking on the window, adds the point to the convex hull (if it's not inside it)
 * and updates the hull, otherwise nothing happens. Every time you click, it will be
 * displayed on the console whether the point could be added or not.
*/

// the path where input will be stored
char path[] = "D:\\Total salvari\\Anul II\\Semestrul I\\Geometrie computationala\\Adding a point to a convex hull - OpenGL project\\data.in";
ifstream fin(path);

// struct to memorize the point's coordinates
struct Point{
    double x , y;
};

// array of points, stored in counter-clockwise order
vector <Point> v;
Point newPoint; // the new point that will be added

int vSize; // size of the v vector

void read(vector <Point> &v , int &n , Point &a)
{
    //read the number of vertex and the vertices
    if(!fin)
    {
        cout<<"Cannot open file\n";
        exit(1);
    }
    fin>>n;
    v.resize(n);
    for(int i = 0 ; i < n ; i++)
        fin>>v[i].x>>v[i].y;

}


double distance(Point A , Point B)
{
    //calculate the square of the distance
    return (A.x - B.x) * (A.x - B.x) + (A.y - B.y) * (A.y - B.y);
}

int orientation(Point p, Point q, Point r)
{
    // The function returns following values
    // 0 --> p, q and r are colinear
    // 1 --> Clockwise
    // -1 --> Counterclockwise

    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);

    if (val == 0) return 0;  // colinear
    return (val > 0)? 1: -1; // clock or counterclock wise
}

int findIndexOfMinDistance(vector <Point> v , int vSize , Point newPoint)
{
    //search the ith vertex with distance(ith vertex , the added point) is minimum
    //return the index with that property

    double minDistance = distance(newPoint, v[0]);
    int indexMinDistance = 0;

    for(int i = 1 ; i < vSize ; i++)
    {
        int newDistance = distance(newPoint, v[i]);
        if(newDistance < minDistance)
        {
            minDistance = newDistance;
            indexMinDistance = i;
        }
    }
    return indexMinDistance;
}

// prints the points
void printVector(vector <Point> v , int vSize)
{
    for(int i = 0 ; i < vSize ; i++)
        cout<<v[i].x<<' '<<v[i].y<<"   ";
}

// Returns true if the point p lies inside the polygon[] with n vertices
bool isInside(vector <Point> v , int vSize, Point newPoint)
{
    // There must be at least 3 vertices in polygon[]
    if (vSize < 3)  return false;

    for(int i = 0 ; i < vSize ; i++)
        {
            if(newPoint.x == v[i].x && newPoint.y == v[i].y) //we consider that a vertex of a polygon is inside it
                return true;
            //we say that a point is inside a polygon
            //if  p is always on the left side of an edge
            if(orientation(newPoint , v[i] , v[(i + 1) % vSize]) >= 0) //clockwise order. pi is not on the left side of edge vi v(i+1)%n
                return false;                               //so p is not inside the polygon
        }
    return true;
}


void addPointToConvexHull(vector <Point> &v , int &vSize , Point newPoint)
{
    // function that adds a point (if it's not inside the convexHull) to the convex Hull, maintaining the
    // points in the counter-clockwise order

    // if the point is inside the convex Hull, nothing happens
    if(isInside(v, vSize, newPoint))
    {
      cout<<"Point " << newPoint.x << " " << newPoint.y <<" is inside\n\n";
      return ;
    }
    else cout<<"Point " << newPoint.x << " " << newPoint.y <<" was added to the convex hull\n";

    int upper = 0; // the upper tangent point from newPoint to the convex hull
    int lower = 0; // the lower tangent point from newPoint to the convex hull

    for(int i = 1 ; i < vSize ; i++)
    {
        int test1 = orientation(newPoint , v[i - 1] , v[i]);
        int test2 = orientation(newPoint , v[i] , v[(i + 1) % vSize]);

        if(test1 >= 0 && test2 < 0)
            upper = i;

        if(test1 < 0 && test2 >= 0)
            lower = i;
    }

    int current = upper;
    vector <Point> aux;
    aux.push_back(v[current]);

    while (current != lower) // adds the points to the convex hull
    {
        current = (current + 1) % vSize;
        aux.push_back(v[current]);
    }

    // Modifies the original vector, keeping only the ones that are part of the convex hull
    aux.push_back(newPoint); // the point will be the last one in the new convex hull
    v.clear();

    for (int i = 0 ; i < aux.size() ; i++)
        v.push_back(aux[i]);
    vSize = aux.size();

    cout<<"The new convex hull (displayed in counter-clockwise order): \n";
    printVector(v, vSize);
    cout<<"\n\n";
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // prints the polygon
    glBegin(GL_POLYGON);
    for(int i = 0 ; i < vSize ;i++)
        glVertex2f(v[i].x , v[i].y);
    glEnd();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFlush();
}

void reshape(GLsizei width, GLsizei height) {

    // Set the viewport to cover the new window
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
    glLoadIdentity();             // Reset the projection matrix
    gluOrtho2D(-width/ 2.0, width/ 2.0,-height / 2.0 ,height / 2.0); // compute the window's size to the those dimensions

}


void mouseClick(int button, int state, int x, int y)
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) //if the left button is pressed
    {
        double windowHeightSize = glutGet(GLUT_WINDOW_HEIGHT); // gets the window's dimensions
        double windowWidthSize = glutGet(GLUT_WINDOW_WIDTH);

        // computes the coordinates of the point, converted for the window's size
        newPoint.x = x - (double)windowWidthSize /2.0;
        newPoint.y = (double)windowHeightSize /2.0- y;
        addPointToConvexHull(v, vSize, newPoint);
        display();
    }
}

int main(int argc,char** argv)
{
    read(v, vSize, newPoint); // reads the convex hull
    glutInit(&argc, argv);
    glutInitWindowSize(800 , 600); // sets the window with the default 800 x 600 dimensions
    glutInitWindowPosition(50 , 50);//sets the position of the window in pixels from top left corner
    glutCreateWindow("Adding a Point to a Convex Hull"); // creates the window
    glutDisplayFunc(display); // function that displays the window
    glutReshapeFunc(reshape); // function that reshapes the window
    glutMouseFunc(mouseClick); // function that tracks the click on the screen
    glutMainLoop(); //loops the current event

    return 0;
}
