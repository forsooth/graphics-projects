#include <math.h>
#include "Raycaster.hpp"
#include "KDTree.hpp"

#define FISH_EYE 1
#define ORTHO 2

Raycaster::Raycaster(const Camera &c, Flattener &f,
                     const SceneParser &p, const KDTree &t,
                     const real W, const real H,
                     const int lens, unsigned int limit,
                     const int kdtree, const int shadows,
                     const int reflections, const int objray,
                     const int pm) : cam(c), flattener(f),
                                                  parser(p), tree(t),
                                                  pixW(W), pixH(H), lens(lens),
                                                  depthlimit(limit),
                                                  kdtree(kdtree),
                                                  shadows(shadows),
                                                  reflections(reflections),
                                                  objray(objray),
                                                  precompute_mats(pm) {
        std::cerr << "Created raycaster." << std::endl;
}

Vector Raycaster::generate_ray(real col, real row) {
        Vector look = cam.GetLookVector();
        look.normalize();

        if (lens == ORTHO) {
                return look;
        } else if (lens == FISH_EYE) {
                real H = DEG_TO_RAD(cam.GetViewAngle());
                real W = H * cam.GetScreenWidthRatio();

                real A = 0.5f * W  + ((-1.0f * W) * (col / (pixW - 1.0f)));
                real B = 0.5f * H + (( -1.0f * H) * (row / (pixH - 1.0f)));

                Vector U = cam.U;
                Vector V = cam.V;

                Matrix rot =  rot_mat(V, A) * rot_mat(U, B);

                return rot * look;
        }

        if (objray) {
                Matrix m = cam.GetFilmToWorldMatrix();

                real A = (2.0 * col / pixW) - 1.0;
                real B = (2.0 * row / pixH) - 1.0;

                Vector ray = m * Vector(A, B, -1);
                ray[1] = ray[1] * -1;
                return normalize(ray);
        } else {
                look = cam.GetNearPlane() * look;
                Point Q = Point(look[0], look[1], look[2]);

                real H = cam.GetNearPlane()
                         * tan(DEG_TO_RAD(cam.GetViewAngle()) / 2.0f);
                real W = H * cam.GetScreenWidthRatio();

                real A = -W + ((2.0f * W) * (col / (pixW - 1.0f)));
                real B = H + ((-2.0f * H) * (row / (pixH - 1.0f)));

                Vector U = cam.U;
                Vector V = cam.V;

                Point S = Q + (A * U) + (B * V);
                S.normalize();
                return S;
        }
}


Intersection Raycaster::get_intersection(const Point &eye, const Vector &ray) {
        Point objEye;
        Vector objRay;
        Intersection isect;
        isect.minT = std::numeric_limits<real>::max();
        real t;
        std::vector<const SceneOp *> f;

        if (kdtree) {
                f = tree.get_intersected(eye, ray);
        } else {
                f = flattener.flattened;
        }

        for (const SceneOp *op : f) {
                if (precompute_mats) {
                        objEye = op->invmat * eye;
                        objRay = op->invmat * ray;
                } else {
                        objEye = invert(op->mat) * eye;
                        objRay = invert(op->mat) * ray;
                }

                for (Shape *s : op->shapes) {
                        t = s->Intersect(objEye, objRay);
                        if (t <= 0 || t >= isect.minT) {
                                continue;
                        }

                        isect.minT = t;
                        isect.minShape = s;
                        isect.minOp = op;
                }
        }

        return isect;
}

SceneColor Raycaster::get_intensity(const SceneMaterial &material,
                                    const Point &isectP,
                                    const Vector &N,
                                    const Vector &worldRay,
                                    Shape *shape,
                                    const SceneOp *op,
                                    const unsigned int depth) {

        SceneGlobalData globalData;
        parser.getGlobalData(globalData);

        /* Ambient lighting constants */
        real k_a = globalData.ka;
        SceneColor O_aλ = material.cAmbient;

        /* Diffuse lighting constants */
        real k_d = globalData.kd;
        SceneColor O_dλ = material.cDiffuse;

        /* Specular lighting constants; not required */
        real k_s = globalData.ks;
        SceneColor O_sλ = material.cSpecular;
        SceneColor O_rλ = material.cReflective;
        real shininess = material.shininess;

        Vector V = normalize(worldRay);

        SceneLightData light;
        SceneColor l_iλ;
        Point lpos;
        Vector L_i;
        real ldist = std::numeric_limits<real>::max();
        real dot_prod_D;
        real dot_prod_S;

        unsigned int m = parser.getNumLights();
        //unsigned int num_hits = 1;

        SceneColor diffuse = {.r=0, .g=0, .b=0, .a=1};
        SceneColor specular = {.r=0, .g=0, .b=0, .a=1};
        SceneColor reflective = {.r=0, .g=0, .b=0, .a=1};

        for (unsigned int i = 0; i < m; i++) {
                parser.getLightData(i, light);
                l_iλ = light.color;
                lpos = light.pos;
                if (light.type == LIGHT_DIRECTIONAL) {
                        L_i = -light.dir;
                } else {
                        L_i = lpos - isectP;
                        ldist = length(L_i);
                }
                L_i.normalize();
                dot_prod_D = dot(N, L_i);

                Point objEye;
                Vector objRay;

                if (shadows) {
                        Intersection intersection = get_intersection(isectP, L_i);
                        real minT = intersection.minT;
                        if (minT >= ldist) {
                                diffuse = diffuse + (real)(dot_prod_D >= 0)
                                          * (l_iλ * dot_prod_D);
                                        // num_hits += (dot_prod_D >= 0);
                        }
                } else {
                        diffuse = diffuse + (real)(dot_prod_D >= 0)
                                            * (l_iλ * dot_prod_D);
                }

                // Specular Lighting
                Vector R_i = normalize(L_i - 2.0f * N * dot_prod_D);

                dot_prod_S = dot(V, R_i);
                specular = specular + (real)(dot_prod_S >= 0) * l_iλ * pow(dot_prod_S, shininess);
        }

        if (!reflections) {
                                reflective = {
                                .r = 0.0f,
                                .g = 0.0f,
                                .b = 0.0f,
                                .a = 1.0f
                        };
        } else {
                Vector R_λ = normalize(worldRay - 2.0f * N * dot(N, worldRay));
                Intersection ref_isect = get_intersection(isectP, R_λ);
                if (depth >= depthlimit ||
                    ref_isect.minT >= std::numeric_limits<real>::max()) {
                        reflective = {
                                .r = 0.0f,
                                .g = 0.0f,
                                .b = 0.0f,
                                .a = 1.0f
                        };
                } else {
                        ScenePrimitive *ref_minPrim = nullptr;
                        for (ScenePrimitive *prim : ref_isect.minOp->objs) {
                                if (prim->type == ref_isect.minShape->type) {
                                        ref_minPrim = prim;
                                        break;
                                }
                        }
                        Point ref_minEye;
                        Vector ref_minRay;
                        if (precompute_mats) {
                                ref_minEye = ref_isect.minOp->invmat * isectP;
                                ref_minRay = ref_isect.minOp->invmat * R_λ;

                        } else {
                                ref_minEye = invert(ref_isect.minOp->mat) * isectP;
                                ref_minRay = invert(ref_isect.minOp->mat) * R_λ;
                        }

                        Vector ref_minNormal =
                                ref_isect.minShape->findIsectNormal(ref_minEye,
                                                   ref_minRay, ref_isect.minT);
                        if (precompute_mats) {
                                ref_minNormal = ref_isect.minOp->invtransmat * ref_minNormal;
                        } else {
                                ref_minNormal = transpose(invert(ref_isect.minOp->mat)) * ref_minNormal;

                        }
                        ref_minNormal.normalize();
                        Point new_isect = isectP + (ref_isect.minT * R_λ);

                        reflective = reflective +  get_intensity(ref_minPrim->material,
                                                                 new_isect, ref_minNormal,
                                                                 R_λ, ref_isect.minShape,
                                                                 ref_isect.minOp, depth + 1);
                }
        }

        SceneColor textureColor;

        if (op->notexture) {
                textureColor = {
                        .r = 0.0,
                        .g = 0.0,
                        .b = 0.0,
                        .a = 1.0
                };
        } else {
                Point unitPlane;
                if (precompute_mats) {
                        unitPlane = shape->to_unit_plane(op->invmat * isectP);
                } else {
                        unitPlane = shape->to_unit_plane(invert(op->mat) * isectP);
                }

                Texture tex = flattener.textures[op->filename];
                unsigned char *pixels = tex.pixels;
                int w = tex.width;
                int h = tex.height;
                float i = tex.i;
                float j = tex.j;
                real u = unitPlane[0];
                real v = unitPlane[1];

                int s = (int)floor(i * u * (w - 1)) % w;
                int t = (int)floor(j * v * (h - 1)) % h;
                int idx = (s * 3) + (w * (t * 3));

                float textureRed = pixels[idx] / 255.0f;
                float textureGreen = pixels[idx + 1] / 255.0f;
                float textureBlue = pixels[idx + 2] / 255.0f;
                textureColor = {
                        .r = textureRed,
                        .g = textureGreen,
                        .b = textureBlue,
                        .a = 1.0
                };
        }

        double blend = material.blend;
        SceneColor I_λ =   (O_aλ * (1.0 - blend) + blend * textureColor) * k_a
                         + (O_dλ * (1.0 - blend) + blend * textureColor) * k_d * diffuse
                         + (O_sλ * (1.0 - blend) + blend * textureColor) * k_s * specular
                         + (O_rλ * (1.0 - blend) + blend * textureColor) * k_s * reflective;

        I_λ.r = fmin(I_λ.r, 1.0f);
        I_λ.g = fmin(I_λ.g, 1.0f);
        I_λ.b = fmin(I_λ.b, 1.0f);
        I_λ.r = fmax(I_λ.r, 0.0f);
        I_λ.g = fmax(I_λ.g, 0.0f);
        I_λ.b = fmax(I_λ.b, 0.0f);

        return I_λ;

}


SceneColor Raycaster::raycast(real col, real row) {

        Vector worldRay = generate_ray(col, row);
        Point worldEye = cam.GetEyePoint();

        if (lens == ORTHO) {
                Vector look = cam.GetLookVector();
                look.normalize();

                look = cam.GetNearPlane() * look;
                Point Q = Point(look[0], look[1], look[2]);

                real H = cam.GetNearPlane()
                         * tan(DEG_TO_RAD(cam.GetViewAngle()) / 2.0f);
                real W = H * cam.GetScreenWidthRatio();

                real A = -W + ((2.0f * W) * (col / (pixW - 1.0f)));
                real B = H + ((-2.0f * H) * (row / (pixH - 1.0f)));

                Vector U = cam.U;
                Vector V = cam.V;

                Point S = worldEye + look + 6e3 * (A * U) + 6e3 * (B * V);

                worldEye = S;
        }

        Intersection isect = get_intersection(worldEye, worldRay);
        real minT = isect.minT;
        const SceneOp *minOp = isect.minOp;
        Shape *minShape = isect.minShape;

        if (minT >= std::numeric_limits<real>::max()) {
                return {
                        .r = 230.0f / 255.0f,
                        .g = 230.0f / 255.0f,
                        .b = 210.0f / 255.0f,
                        .a = 1.0f
                };
        } else {
                ScenePrimitive *minPrim = nullptr;
                for (ScenePrimitive *prim : minOp->objs) {
                        if (prim->type == minShape->type) {
                                minPrim = prim;
                                break;
                        }
                }

                Point minEye;
                Vector minRay;

                if (precompute_mats) {
                        minEye = minOp->invmat * worldEye;
                        minRay = minOp->invmat * worldRay;
                } else {
                        minEye = invert(minOp->mat) * worldEye;
                        minRay = invert(minOp->mat) * worldRay;
                }
                Vector minNormal = minShape->findIsectNormal(minEye,
                                                             minRay, minT);
                if (precompute_mats) {
                        minNormal = minOp->invtransmat * minNormal;
                } else {
                        minNormal = transpose(invert(minOp->mat)) * minNormal;
                }
                minNormal.normalize();
                Point isectP = worldEye + (1 - 1e-4f) * (minT * worldRay);
                // TODO: null checks
                return get_intensity(minPrim->material, isectP, minNormal,
                                     worldRay, isect.minShape, minOp, 0);
        }
}
