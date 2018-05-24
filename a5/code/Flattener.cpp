#include "Flattener.hpp"

Flattener::Flattener() {}
Flattener::~Flattener() {}

Cube *cube = new Cube();
Cylinder *cylinder = new Cylinder();
Cone *cone = new Cone();
Sphere *sphere = new Sphere();

Shape *select_shape(int shapeType) {
        switch (shapeType) {
                case SHAPE_CUBE:
                        return cube;
                case SHAPE_CYLINDER:
                        return cylinder;
                case SHAPE_CONE:
                        return cone;
                case SHAPE_SPHERE:
                        return sphere;
                case SHAPE_SPECIAL1:
                        return cube;
                default:
                        return cube;
        }
}

void Flattener::flattenTree(SceneParser &parser) {
        for (const SceneOp *op : flattened) {
            delete op;
        }
        flattened.clear();
        fprintf(stderr, "Generated flattened scene tree\n");
        SceneNode *root = parser.getRootNode();
        if (root == NULL) {
                return;
        }
        flattenChild(root);

        std::vector<const SceneOp *> expanded_flat = flattened;
        flattened.clear();

        std::vector<Matrix> matrices;
        for (const SceneOp *op : expanded_flat) {

                matrices.emplace_back(op->mat);
                Matrix acc = Matrix();
                for (unsigned int i = 0; i < matrices.size(); i++) {
                        acc = acc * matrices[i];
                }

                std::vector<Shape *> shapes;

                for (ScenePrimitive *prim : op->objs) {
                        Shape *s = select_shape(prim->type);
                        if (!(std::find(shapes.begin(), shapes.end(), s) != shapes.end())) {
                                s->type = prim->type;
                                shapes.emplace_back(s);
                        }
                }

                SceneOp *compressed;
                id += 1;

                if (op->objs.size() == 0 || op->objs[0]->material.textureMap->filename.empty()) {
                        compressed = new SceneOp({true,
                                                  id,
                                                  acc,
                                                  invert(acc),
                                                  transpose(invert(acc)),
                                                  "",
                                                  op->objs,
                                                  shapes
                                                });
                } else {
                        std::string fname = op->objs[0]->material.textureMap->filename;
                        Texture texture;
                        std::map<std::string, Texture>::iterator it = textures.find(fname);
                        if (it == textures.end()) {
                                ppm texture_ppm = ppm(fname);
                                texture.pixels = (unsigned char *)texture_ppm.getPixels();
                                texture.width = texture_ppm.getWidth();
                                texture.height = texture_ppm.getHeight();
                                texture.i = op->objs[0]->material.textureMap->repeatU;
                                texture.j = op->objs[0]->material.textureMap->repeatV;
                                textures[fname] = texture;
                        } else {
                                texture = it->second;
                        }
                        compressed = new SceneOp({false,
                                                  id,
                                                  acc,
                                                  invert(acc),
                                                  transpose(invert(acc)),
                                                  fname,
                                                  op->objs,
                                                  shapes
                                                });
                }

                if (!compressed->objs.empty()) {
                        flattened.emplace_back(compressed);
                }

                for (uint j = 0; j < op->pops; j++) {
                        matrices.pop_back();
                }
        }

        for (const SceneOp *op : expanded_flat) {
                delete op;
        }
}

void Flattener::flattenChild(SceneNode *node) {

        Matrix mat;
        for (SceneTransformation *transformation : node->transformations) {
                if (transformation->type == TRANSFORMATION_TRANSLATE) {
                        mat = mat *
                                         trans_mat(transformation->translate);
                } else if (transformation->type == TRANSFORMATION_ROTATE) {
                        mat = mat *
                                         rot_mat(transformation->rotate,
                                                 transformation->angle);
                } else if (transformation->type == TRANSFORMATION_SCALE) {
                        mat = mat *
                                         scale_mat(transformation->scale);
                } else if (transformation->type == TRANSFORMATION_MATRIX) {
                        mat = mat *
                                         transformation->matrix;
                }
        }


        if (node->children.empty()) {
                SceneOp *flat_elem = new SceneOp({false,
                                                  1,
                                                  mat,
                                                  Matrix(),
                                                  Matrix(),
                                                  "",
                                                  node->primitives,
                                                  std::vector<Shape *>()});
                flattened.emplace_back(flat_elem);
                return;
        }

        SceneOp *flat_elem = new SceneOp({false,
                                          0,
                                          mat,
                                          Matrix(),
                                          Matrix(),
                                          "",
                                          node->primitives,
                                          std::vector<Shape *>()});
        flattened.emplace_back(flat_elem);

        for (unsigned int i = 0; i < node->children.size(); i++) {
                flattenChild(node->children[i]);
                if (i == node->children.size() - 1) {
                        const SceneOp *oldop = flattened[flattened.size() - 1];
                        SceneOp *newop = new SceneOp({false,
                                                      oldop->pops + 1,
                                                      oldop->mat,
                                                      Matrix(),
                                                      Matrix(),
                                                      "",
                                                      oldop->objs,
                                                      std::vector<Shape *>()});
                        flattened[flattened.size() - 1] = newop;
                }
        }
}
