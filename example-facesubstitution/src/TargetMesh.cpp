#include "TargetMesh.h"

TargetMesh::TargetMesh(){
    framesUnseen = 0;

    // init the kalman filters
    for(int i = 0; i < 86; i++){
        positions[i].init(1/800., 1/30., true); // smoothness, rapidness (smaller is more mooth/rapid)
    }
    preloaded = false;
    
    // add triangles or these vertices
    // outside left
    addTriangle(0,41,1);
    addTriangle(1,41,31);
    addTriangle(1,31,2);
    addTriangle(2,31,49);
    addTriangle(2,49,3);
    addTriangle(3,49,48);
    addTriangle(3,48,4);
    addTriangle(4,48,5);
    addTriangle(5,48,59);
    addTriangle(5,59,6);
    addTriangle(6,59,58);
    addTriangle(6,58,7);
    addTriangle(7,58,57);
    addTriangle(7,57,8);
    // outside right
    addTriangle(8,57,56);
    addTriangle(8,56,9);
    addTriangle(9,56,10);
    addTriangle(10,56,55);
    addTriangle(10,55,11);
    addTriangle(11,55,54);
    addTriangle(11,54,12);
    addTriangle(12,54,13);
    addTriangle(13,54,53);
    addTriangle(13,53,14);
    addTriangle(14,53,35);
    addTriangle(14,35,15);
    addTriangle(15,35,46);
    addTriangle(15,46,16);
    // mouth bottom
    addTriangle(48,60,59);
    addTriangle(59,60,67);
    addTriangle(59,67,58);
    addTriangle(58,67,66);
    addTriangle(58,66,57);
    addTriangle(57,66,56);
    addTriangle(56,66,65);
    addTriangle(56,65,55);
    addTriangle(55,65,64);
    addTriangle(55,64,54);
    // mouth upper
    addTriangle(48,49,60);
    addTriangle(49,61,60);
    addTriangle(49,50,61);
    addTriangle(50,51,61);
    addTriangle(61,51,62);
    addTriangle(51,63,62);
    addTriangle(51,52,63);
    addTriangle(52,53,63);
    addTriangle(63,53,64);
    addTriangle(64,53,54);
    // mouth inner
    addTriangle(60,61,67);
    addTriangle(61,62,67);
    addTriangle(62,66,67);
    addTriangle(66,62,65);
    addTriangle(65,62,63);
    addTriangle(65,63,64);
    // upper lip
    addTriangle(49,31,50);
    addTriangle(50,31,32);
    addTriangle(50,32,33);
    addTriangle(50,33,51);
    addTriangle(51,33,52);
    addTriangle(52,33,34);
    addTriangle(52,34,35);
    addTriangle(52,35,53);
    // nose bottom
    addTriangle(31,30,32);
    addTriangle(32,30,33);
    addTriangle(33,30,34);
    addTriangle(34,30,35);
    // eyes
    addTriangle(0,36,41);
    addTriangle(0,17,36);
    addTriangle(17,37,36);
    addTriangle(17,18,37);
    addTriangle(18,19,37);
    addTriangle(19,38,37);
    addTriangle(19,20,38);
    addTriangle(20,21,38);
    addTriangle(21,39,38);
    addTriangle(21,27,39);
    addTriangle(39,27,28);
    addTriangle(39,28,29);
    addTriangle(40,39,31);
    addTriangle(39,29,31);
    addTriangle(29,30,31);
    addTriangle(41,40,31);
    addTriangle(36,37,41);
    addTriangle(37,40,41);
    addTriangle(37,38,40);
    addTriangle(38,39,40);
    addTriangle(16,46,45);
    addTriangle(16,45,26);
    addTriangle(26,45,25);
    addTriangle(45,44,25);
    addTriangle(44,24,25);
    addTriangle(44,43,24);
    addTriangle(43,23,24);
    addTriangle(43,22,23);
    addTriangle(42,22,43);
    addTriangle(27,22,42);
    addTriangle(27,42,28);
    addTriangle(28,42,29);
    addTriangle(29,42,35);
    addTriangle(42,47,35);
    addTriangle(47,46,35);
    addTriangle(42,43,47);
    addTriangle(47,43,44);
    addTriangle(44,46,47);
    addTriangle(44,45,46);
    addTriangle(30,29,35);
    addTriangle(21,22,27);
    addTriangle(20,22,21);
    addTriangle(20,23,22);
}

void TargetMesh::update_vertices(const std::vector<glm::vec2> &points){
    framesUnseen = 0;
    clearVertices();
    int i = 0;
    for(std::vector<glm::vec2>::const_iterator it = points.begin(); it != points.end(); ++it){
        if(!preloaded){
            positions[i].initialvals(it->x,it->y,0.0f);
        }
        glm::vec3 tempIt = {it->x,it->y,0.0f};
        positions[i].update(tempIt);
        glm::vec2 pos = positions[i].getEstimation();
        addVertex(glm::vec3(pos.x,pos.y,0.0f));
        //addVertex(glm::vec3(it->x,it->y,0.0f)); // direct, without Kalman filter
        ++i;
    }
}

void TargetMesh::update_uvs(const std::vector<glm::vec2> &points){
    clearTexCoords();
    for(std::vector<glm::vec2>::const_iterator it = points.begin(); it != points.end(); ++it){
        addTexCoord(*it);
    }
}

void TargetMesh::render(){
    framesUnseen += 1;
    draw();
}
