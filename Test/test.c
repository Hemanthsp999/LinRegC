#include "../EDA/DataAnalysis.h"
#include "../Regression/Linear.h"
#include <float.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Enter File Name
const char *FileName = "Datasets/winequality-white.csv";

int main() {
        // Read file from the Dataset
        printf(
            "Hi, Welcome to LinRegC Library: A simple and efficient tool for "
            "simple linear regression\n");

        /*FileName, IndependentVar, DependentVar */
        getFile *data = Read_Dataset(FileName, 10, 11);

        int datasize = data->num_rows;

        float y_min, y_max;
        // Pass the data points to Normalization function.
        NormVar *normalize =
            Normalize(data->X, data->Y, datasize, &y_min, &y_max);

        size_t size_x, size_y;
        size_x = size_y = datasize;

        float train_ratio = 0.8;
        // Facing problem  in split_data
        SplitData *split_data = (SplitData *)malloc(sizeof(SplitData));
        *split_data =
            Split_Dataset(normalize->X, normalize->Y, size_x, train_ratio);

        Beta *model =
            Fit_Model(split_data->X_Train, split_data->Y_Train, size_x, size_y);

        // Learning rate
        float lr = 0.01;
        int epochs = 10000;
        printf("Slope: %.2f\t Intercept: %.2f\n", model->slope,
               model->intercept);

        Stochastic_Gradient_Descent(normalize->X, normalize->Y, model, size_x,
                                    epochs, lr);
        printf("\n\t\tAfter stochastic gradient\t\n Slope: %.2f\t Intercept: "
               "%.2f \t\n",
               model->slope, model->intercept);

        float *prediction = Predict_Model(split_data->X_Test, datasize, *model);

        float *cost = (float *)malloc(sizeof(float));
        if (!cost) {
                fprintf(stderr,
                        "Test().*Cost Error: Memory Allocation Error\n");
        }

        // convert Normalized X_Test to Denormalized X_Test
        // Fixed Bug in Cost_Function
        *cost = Cost_Function(split_data->Y_Test, prediction, size_x, size_y);
        float *prediction_denorm_var =
            Denormalize(prediction, y_min, y_max, size_x);
        if (!prediction_denorm_var) {
                fprintf(stderr,
                        "Test().denormVar Error: Memory allocation failed\n");
                return EXIT_FAILURE;
        }
        float *Y_test = Denormalize(split_data->Y_Test, y_min, y_max, size_y);
        if (!Y_test) {
                fprintf(stderr,
                        "Test().Y_Test Error: Memory allocation failed\n");
                free(Y_test);
        }

        for (int i = 0; i < datasize; i++) {
                printf("Independent Var: %.2f\t Dependent var: %.2f == Actual "
                       "element: "
                       "%.2f |\t "
                       "Normalized element X: %.2f  Y: %.2f Denormalize "
                       "element: %f\t "
                       "| Predicted element : %.2f |\t Cost_Function: %.2f\n",
                       data->X[i], data->Y[i], Y_test[i], normalize->X[i],
                       normalize->Y[i], prediction_denorm_var[i], prediction[i],
                       cost[i]);
        }

        metricResult rmse = RMSE(split_data->Y_Test, prediction, size_x);
        if (rmse.is_valid) {
                printf("RMSE: Is_Valid(0: No 1: Yes) ? %d Accuracy: %f",
                       rmse.is_valid, rmse.Accuracy);
        } else {
                fprintf(stderr, "Failed to calculate RMSE\n");
                return 0;
        }

        metricResult mse = MSE(split_data->Y_Test, prediction, size_x);
        if (mse.Accuracy >= 0) {

                printf("\tMSE: Is_Valid(0: No 1: Yes) ? %d\t Accuracy: %f\n",
                       mse.is_valid, mse.Accuracy);
        } else {
                printf("Failed to calculate MSE\n");
                return 0;
        }

        // Free the memory allocations
        Free_Model(model);
        Free_Data(data);
        Free_Normalize(normalize);
        Free_Split(split_data);
        free(prediction);
        free(prediction_denorm_var);
        free(Y_test);
        free(cost);

        return 0;
}
