#-------------------------------------------------
#
#  TP Bezier surfaces
#
#-------------------------------------------------

import sys, os
import numpy as np
from OpenGL.GL import *
from viewer.viewer import Viewer

TP = os.path.dirname(os.path.realpath(__file__)) + "/"
DATADIR = TP+"data/"

#-------------------------------------------------
# READBEZIERMESH()
# Read Bezier control net from a BPT file.
#
# Input
#    filename    :  file to be read
#
# Output
#    Mx, My, Mz  :  three matrices, one for each coordinate of the control mesh
#
def ReadBezierMesh(datafile):
    line = datafile.readline()
    deg = np.fromstring(line, sep=' ', dtype=int)
    M = np.fromfile(datafile, count=3*(deg[0]+1)*(deg[1]+1), sep=' ', dtype=float)
    M = M.reshape(-1, 3).transpose().reshape(3, deg[0]+1, deg[1]+1)
    return M[0, :, :], M[1, :, :], M[2, :, :]


#-------------------------------------------------
# DECASTELJAU( ... )
# Compute point on a Bezier surface using the De Casteljau algorithm.
#
# Input
#    M        :  m x n matrix, control mesh (one coordinate)
#    u, v     :  parameters
#
# Output      :  one coordinate of the surface point S(u,v)
#
def DeCasteljau(M,u,v):
    
    #
    # TODO Implement the De Casteljau algorithm for surfaces.
    #
    # hint:
    # The above signature is for non-recursive implementation.
    # For recursive implementation, you can use
    #   def DeCasteljau(M,k,l,i,j,u,v) :
    #
    k = M.shape[0] - 1
    l = M.shape[1] - 1
    return DeCasteljauRec(M, k, l, 0, 0, u, v)

def DeCasteljauRec(M, k, l, i, j, u, v):

    #Condition d'arrêt
    if k == 0 and l == 0:
        return M[i, j]

    # Si on peut encore réduire en u
    if k > 0:
        val1 = DeCasteljauRec(M, k - 1, l, i, j, u, v)
        val2 = DeCasteljauRec(M, k - 1, l, i + 1, j, u, v)
        return (1 - u) * val1 + u * val2

    # Sinon, on réduit en v
    if l > 0:
        val1 = DeCasteljauRec(M, k, l - 1, i, j, u, v)
        val2 = DeCasteljauRec(M, k, l - 1, i, j + 1, u, v)
        return (1 - v) * val1 + v * val2

#-------------------------------------------------
# BEZIERSURF( ... )
# Compute Bezier surface points.
#
# Input
#    M        :  m x n matrix, control mesh (one coordinate)
#    density  :  sampling density
#
# Output
#    S        :  density x density matrix, surface points (one coordinate)
#
# Note :
# This function is later called three times,
# for each 3d coordinate individually.
#

def BezierSurf(M,density) :
    
    # surface degrees
    m, n = M.shape - np.array([1,1])
    
    # init surface points
    S = np.zeros([density, density])

    #
    # TODO Fill surface points.
    #
    #
    # hint:
    # to generate uniform sampling of the interval [0.0,1.0], use:
    # >> u = np.linspace(0.0,1.0,num=density)
    #
    
    u = np.linspace(0.0,1.0,num=density)
    v = np.linspace(0.0,1.0,num=density)
    
    for i in range(density):
        for j in range(density):
            S[i,j] = DeCasteljau(M,u[i],v[j])
    
    return S


#-------------------------------------------------
if __name__ == "__main__":
    
    # arg 1 : data name
    if len(sys.argv) > 1:
        dataname = sys.argv[1]
    else:
        dataname = "simple"

    # arg 2 : sampling density
    if len(sys.argv) > 2:
        density = int(sys.argv[2])
    else:
        density = 10

    # filename
    filename = DATADIR+dataname+".bpt"
    print(filename)
    
    # check if valid datafile
    if not os.path.isfile(filename):
        print(" error   :  invalid dataname '" + dataname + "'")
        print(" usage   :  tp6.py  [simple,wave,sphere,heart,teapot,teacup,teaspoon]  [density=10]")
        print(" example :  python tp6.py wave 20")
        
    else :
        # open the datafile
        datafile = open(filename, 'r')
        
        # get first line = number of patches
        numpatch = np.fromstring(datafile.readline(), sep=' ', dtype=int)[0]
        print(numpatch)
        
        # init Viewer
        viewer = Viewer("TP : Bezier surfaces ["+dataname+"]", [1200, 800])

        # read and compute each patch
        for p in range(numpatch):
            
            # print patch id
            print(" patch", p+1, "/", numpatch)
            
            # read Bezier control points
            Mx, My, Mz = ReadBezierMesh(datafile)
            
            # compute surface points
            # TODO: change the four next uncommented lines by
            #
            Sx = BezierSurf(Mx, density)
            Sy = BezierSurf(My, density)
            Sz = BezierSurf(Mz, density)
            #
            # once you have implemented BezierSurf()
            #

            # x = np.linspace(-5, 5, density)
            # y = np.linspace(-5, 5, density)
            # Sx, Sy = np.meshgrid(x, y) 
            # Sz = np.zeros((density, density))

            # add patch to the Viewer
            viewer.add_patch(Sx, Sy, Sz)

        # print final message
        print(" done.")
        
        # display the viewer
        viewer.render()
