BVHNode::BVHNode(KDNode *root) {
        
}

BVHNode* BVHNode::construct_tree(KDNode *kdnode) {
        if (kdnode == nullptr) {
                return nullptr;
        }

        left = construct_tree(kdnode->left);
        right = construct_tree(kdnode->right);

        if (kdnode->sceneops.size() != 0){
                // leaf
                sceneops = kdnode->sceneops;
                bounding_box = 
        }

}

