#include "prac01/surf.h"
#include "prac01/extra.h"
using namespace std;

namespace
{
    
    // We're only implenting swept surfaces where the profile curve is
    // flat on the xy-plane.  This is a check function.
    static bool checkFlat(const Curve &profile)
    {
        for (unsigned i=0; i<profile.size(); i++)
            if (profile[i].V[2] != 0.0 ||
                profile[i].T[2] != 0.0 ||
                profile[i].N[2] != 0.0)
                return false;
    
        return true;
    }
}

vector<Tup3u> makeFaces(int x, int y) {
    auto loc = [y](int u, int v) { return u * y + v; };

    vector<Tup3u> faces;
    faces.reserve(2 * (x - 1) * (y - 1));

    for (unsigned u = 1; u < x; u++) {
        for (unsigned v = 1; v < y; v++) {
            faces.emplace_back(loc(u, v - 1),     //
                               loc(u - 1, v - 1), //
                               loc(u - 1, v));
            faces.emplace_back(loc(u - 1, v), //
                               loc(u, v),     //
                               loc(u, v - 1));
        }
    }

    return faces;
}

Surface makeSurfRev(const Curve &profile, unsigned steps)
{
    Surface surface;
    
    if (!checkFlat(profile))
    {
        cerr << "surfRev profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.

    for (unsigned u = 0; u <= steps; u++) {
        auto angle = 2 * M_PI * static_cast<float>(u) / steps;
        auto rotation = Matrix3f::rotateY(angle);

        for (unsigned v = 0; v < profile.size(); v++) {
            auto q = profile[v];
            surface.VV.emplace_back(rotation * q.V);
            surface.VN.emplace_back(rotation * -q.N);
        }
    }

    surface.VF = makeFaces(steps + 1, profile.size());

    //cerr << "\t>>> makeSurfRev called (but not implemented).\n\t>>> Returning empty surface." << endl;
 
    return surface;
}

Surface makeGenCyl(const Curve &profile, const Curve &sweep )
{
    Surface surface;

    if (!checkFlat(profile))
    {
        cerr << "genCyl profile curve must be flat on xy plane." << endl;
        exit(0);
    }

    // TODO: Here you should build the surface.  See surf.h for details.
        for (unsigned u = 0; u < sweep.size(); u++) {
        Matrix4f coord{
            {sweep[u].N, 0},
            {sweep[u].B, 0},
            {sweep[u].T, 0},
            {sweep[u].V, 1},
        };

        // Explanation:
        // https://paroj.github.io/gltut/Illumination/Tut09%20Normal%20Transformation.html
        auto normalRotation =
            coord.getSubmatrix3x3(0, 0).transposed().inverse();

        for (unsigned v = 0; v < profile.size(); v++) {
            surface.VV.emplace_back((coord * Vector4f{profile[v].V, 1}).xyz());
            surface.VN.emplace_back(normalRotation * -profile[v].N);
        }
    }

    surface.VF = makeFaces(sweep.size(), profile.size());

    //cerr << "\t>>> makeGenCyl called (but not implemented).\n\t>>> Returning empty surface." <<endl;

    return surface;
}

void drawSurface(const Surface &surface, bool shaded)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    if (shaded)
    {
        // This will use the current material color and light
        // positions.  Just set these in drawScene();
        glEnable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {        
        glDisable(GL_LIGHTING);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        glColor4f(0.4f,0.4f,0.4f,1.f);
        glLineWidth(1);
    }

    glBegin(GL_TRIANGLES);
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        glNormal(surface.VN[surface.VF[i][0]]);
        glVertex(surface.VV[surface.VF[i][0]]);
        glNormal(surface.VN[surface.VF[i][1]]);
        glVertex(surface.VV[surface.VF[i][1]]);
        glNormal(surface.VN[surface.VF[i][2]]);
        glVertex(surface.VV[surface.VF[i][2]]);
    }
    glEnd();

    glPopAttrib();
}

void drawNormals(const Surface &surface, float len)
{
    // Save current state of OpenGL
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_LIGHTING);
    glColor4f(0,1,1,1);
    glLineWidth(1);

    glBegin(GL_LINES);
    for (unsigned i=0; i<surface.VV.size(); i++)
    {
        glVertex(surface.VV[i]);
        glVertex(surface.VV[i] + surface.VN[i] * len);
    }
    glEnd();

    glPopAttrib();
}

void outputObjFile(ostream &out, const Surface &surface)
{
    
    for (unsigned i=0; i<surface.VV.size(); i++)
        out << "v  "
            << surface.VV[i][0] << " "
            << surface.VV[i][1] << " "
            << surface.VV[i][2] << endl;

    for (unsigned i=0; i<surface.VN.size(); i++)
        out << "vn "
            << surface.VN[i][0] << " "
            << surface.VN[i][1] << " "
            << surface.VN[i][2] << endl;

    out << "vt  0 0 0" << endl;
    
    for (unsigned i=0; i<surface.VF.size(); i++)
    {
        out << "f  ";
        for (unsigned j=0; j<3; j++)
        {
            unsigned a = surface.VF[i][j]+1;
            out << a << "/" << "1" << "/" << a << " ";
        }
        out << endl;
    }
}
