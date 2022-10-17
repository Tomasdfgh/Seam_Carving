Seam Carving

Seam Carving is an image processing technique that discard the low energy pixels while keeping the "higher energy" ones. This process allow the user to keep the most important parts of the picture while discarding the less important ones. This project implemented C inorder to find the veticle path that has the lowest energy pixels for removal.

How this algorithm works is by making use of dynamic programming. First, a matrix is created for individual values of the pixel. Each individual pixel value is the difference in RGB values from its neighbouts. The higher the difference means teh greater the energy. After the matrix ix created, the matrix of energy can then be analyzed and a path of lowest energy from top to bottom is selected to be removed. This process can be repeated in order to produce an image with maximimum artistic value.
