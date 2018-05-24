#include <math.h>
#include "Raycaster.hpp"
#include "KDTree.hpp"

#define PERSECTIVE 0
#define FISH_EYE 1
#define ORTHO 2

Vector generate_ray(real col, real row,
                    real pixW, real pixH, const Camera *cam, int lens) {
        Vector look = cam->GetLookVector();
        look.normalize();

        if (lens == ORTHO) {
                return look;
        } else if (lens == FISH_EYE) {
                real H = DEG_TO_RAD(cam->GetViewAngle());
                real W = H * cam->GetScreenWidthRatio();

                real A = 0.5*W  + ((-1.0f * W) * (col / (pixW - 1.0f)));
                real B = 0.5*H + (( -1.0f * H) * (row / (pixH - 1.0f)));

                Vector U = cam->U;
                Vector V = cam->V;

                Matrix rot =  rot_mat(V, A) * rot_mat(U, B);

                return rot * look;
        }

#ifdef OBJRAY
        Matrix m = cam->GetFilmToWorldMatrix();

        real A = (2.0 * col / pixW) - 1.0;
        real B = (2.0 * row / pixH) - 1.0;

        Vector ray = m * Vector(A, B, -1);
        ray[1] = ray[1] * -1;
        return normalize(ray);
#else
        look = cam->GetNearPlane() * look;
        Point Q = Point(look[0], look[1], look[2]);

        real H = cam->GetNearPlane()
                 * tan(DEG_TO_RAD(cam->GetViewAngle()) / 2.0f);
        real W = H * cam->GetScreenWidthRatio();

        real A = -W + ((2.0f * W) * (col / (pixW - 1.0f)));
        real B = H + ((-2.0f * H) * (row / (pixH - 1.0f)));

        Vector U = cam->U;
        Vector V = cam->V;

        Point S = Q + (A * U) + (B * V);

        Vector ray = S;
        ray.normalize();
        return ray;
#endif
}

SceneColor get_intensity(const SceneMaterial &material, const Point &isect,
                         const Vector &N, const Vector &worldRay,
                         const SceneParser *parser, const SceneOp *thisOp,
                         PrimitiveType thisType, const KDTree *tree) {

        SceneGlobalData globalData;
        parser->getGlobalData(globalData);

        /* Ambient lighting constants */
        real k_a = globalData.ka;
        SceneColor O_aλ = material.cAmbient;

        /* Diffuse lighting constants */
        real k_d = globalData.kd;
        SceneColor O_dλ = material.cDiffuse;

        /* Specular lighting constants; not required */
        real k_s = globalData.ks;
        SceneColor O_sλ = material.cSpecular;
        real f = material.shininess;

        /* Initialize intensity to ambient light value */
        SceneColor ambient = O_aλ * k_a;

        Vector V = normalize(worldRay);

        SceneLightData light;
        SceneColor l_iλ;
        Point lpos;
        Vector L_i;
        real dot_prod_D;
        real dot_prod_S;

        unsigned int m = parser->getNumLights();
        //unsigned int num_hits = 1;

        SceneColor diffuse = {.r=0, .g=0, .b=0, .a=1};
        SceneColor specular = {.r=0, .g=0, .b=0, .a=1};

        for (unsigned int i = 0; i < m; i++) {
                parser->getLightData(i, light);
                l_iλ = light.color;
                lpos = light.pos;
                if (light.type == LIGHT_DIRECTIONAL) {
                        L_i = light.dir;
                } else {
                        L_i = lpos - isect;
                }
                L_i.normalize();

                const std::vector<const SceneOp *> flattened =
                                     tree->get_intersected(lpos, -1 * L_i);
                real minT = std::numeric_limits<real>::max();
                Shape *minShape = nullptr;
                const SceneOp *minOp = nullptr;
                Point objEye;
                Vector objRay;
                real t;

                for (const SceneOp *op : flattened) {
                        objEye = op->invmat * lpos;
                        objRay = op->invmat * (-1 * L_i);

                        for (Shape *s : op->shapes) {
                                t = s->Intersect(objEye, objRay);
                                if (t <= 0 || t >= minT) {
                                        continue;
                                }

                                minT = t;
                                minShape = s;
                                minOp = op;
                        }
                }

                if (minOp == thisOp && minShape->type == thisType) {
                        dot_prod_D = dot(N, L_i);
                        //num_hits += (dot_prod_D >= 0);
                        diffuse = diffuse + (real)(dot_prod_D >= 0)
                                            * ((l_iλ * (O_dλ * k_d))
                                                * dot_prod_D);
                }

                //Specular Lighting
                Vector R_i = normalize(L_i - 2.0f * N * dot_prod_D);

                dot_prod_S = dot(V, R_i);
                specular = specular + (real)(dot_prod_S >= 0)
                                      * (O_sλ * k_s)
                                      * pow(dot_prod_S, f);
        }

        SceneColor I_λ = ambient + diffuse + specular;

        I_λ.r = fmin(I_λ.r, 1.0f);
        I_λ.g = fmin(I_λ.g, 1.0f);
        I_λ.b = fmin(I_λ.b, 1.0f);
        I_λ.r = fmax(I_λ.r, 0.0f);
        I_λ.g = fmax(I_λ.g, 0.0f);
        I_λ.b = fmax(I_λ.b, 0.0f);

        return I_λ;
}

SceneColor raycast(real col, real row, real pixW, real pixH, const Camera *cam,
                   const SceneParser *parser, const KDTree *tree, int lens) {

        Vector worldRay = generate_ray(col, row, pixW, pixH, cam, lens);
        Point worldEye = cam->GetEyePoint();

        if(lens == ORTHO) {
                Vector look = cam->GetLookVector();
                look.normalize();

                look = cam->GetNearPlane() * look;
                Point Q = Point(look[0], look[1], look[2]);

                real H = cam->GetNearPlane()
                         * tan(DEG_TO_RAD(cam->GetViewAngle()) / 2.0f);
                real W = H * cam->GetScreenWidthRatio();

                real A = -W + ((2.0f * W) * (col / (pixW - 1.0f)));
                real B = H + ((-2.0f * H) * (row / (pixH - 1.0f)));

                Vector U = cam->U;
                Vector V = cam->V;

                Point S = worldEye + look + 6e3*(A * U) + 6e3*(B * V);

                worldEye = S;
        }

        const std::vector<const SceneOp *> flattened = 
                                     tree->get_intersected(worldEye, worldRay);

        real minT = std::numeric_limits<real>::max();
        Shape *minShape = nullptr;
        const SceneOp *minOp = nullptr;
        Point objEye;
        Vector objRay;
        real t;

       for (const SceneOp *op : flattened) {
                objEye = op->invmat * worldEye;
                objRay = op->invmat * worldRay;

               for (Shape *s : op->shapes) {
                        t = s->Intersect(objEye, objRay);
                        if (t <= 0 || t >= minT) {
                                continue;
                        }

                        minT = t;
                        minShape = s;
                        minOp = op;
                }
        }

        if (minT >= std::numeric_limits<real>::max()) {
                SceneColor c;
                c.r = 230.0f / 255.0f;
                c.g = 230.0f / 255.0f;
                c.b = 210.0f / 255.0f;
                return c;
        } else {
                ScenePrimitive *minPrim = nullptr;
                for (ScenePrimitive *prim : minOp->objs) {
                        if (prim->type == minShape->type) {
                                minPrim = prim;
                                break;
                        }
                }

                Point minEye = minOp->invmat * worldEye;
                Vector minRay = minOp->invmat * worldRay;
                Vector minNormal = minShape->findIsectNormal(minEye,
                                                             minRay, minT);
                minNormal = minOp->invtransmat * minNormal;
                minNormal.normalize();
                Point isect = worldEye + (minT * worldRay);
                return get_intensity(minPrim->material, isect,
                                     minNormal, worldRay, parser, minOp,
                                     minShape->type, tree);
        }
}
