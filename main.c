#include "functions.h"

int main(int argc, char **argv){
    int my_rank=0, p=0, source=0, dest=0, x=0;

    complex **data1, **data2, **data3, **data4;
    data1 = malloc(N * sizeof(complex *));
    data2 = malloc(N * sizeof(complex *));
    data3 = malloc(N * sizeof(complex *));
    data4 = malloc(N * sizeof(complex *));

    for(x = 0; x < N; x++){
        data1[x] = malloc(N * sizeof(complex *));
        data2[x] = malloc(N * sizeof(complex *));
        data3[x] = malloc(N * sizeof(complex *));
        data4[x] = malloc(N * sizeof(complex *));
    }

    complex *vec;

    // Input Matrices
    char input_matrix_1[21] = "input/sample_matrix_1"; 
    char input_matrix_2[21] = "input/sample_matrix_2";
    // Output Matrix
    char output_matrix[15] = "omp.out";

    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Comm_size(MPI_COMM_WORLD, &p);

    /* Setup description of the 4 MPI_FLOAT fields x, y, z, velocity */
    MPI_Datatype mystruct;
    int          blocklens[2] = { 1, 1 };
    MPI_Aint     indices[2] = { 0, sizeof(float) };
    MPI_Datatype old_types[2] = { MPI_FLOAT, MPI_FLOAT };

    /* Make relative */
    MPI_Type_create_struct( 2, blocklens, indices, old_types, &mystruct );
    MPI_Type_commit( &mystruct );


    int i,j;

    double startTime, stopTime;

    //Starting and send rows of data1, data2

    int offset;

    int tag = 345;

    int rows = N/p;

    int lb = my_rank*rows;
    int hb = lb + rows;

    printf("%d have lb = %d and hb = %d\n", my_rank, lb, hb);

    //Starting and send rows of data1, data2


    if(my_rank == 0){
        getData(input_matrix_1, data1);
        getData(input_matrix_2, data2);

        /* Start Clock */
        printf("\nStarting clock.\n");
        startTime = MPI_Wtime();

        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Send(&data1[j][0], N, mystruct, i, tag, MPI_COMM_WORLD);
                MPI_Send(&data2[j][0], N, mystruct, i, tag, MPI_COMM_WORLD);
            }
        }
    }else{

        for(j = lb; j < hb; j++){
            MPI_Recv(data1[j], N, mystruct, 0, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(data2[j], N, mystruct, 0, tag, MPI_COMM_WORLD, &status);
        }
    }

    //Doing fft1d forward for data1 and data2 rows

    vec = (complex *)malloc(N * sizeof(complex));

    #pragma omp parallel for shared(data1,vec) private(i)
    for (i=lb;i<hb;i++) {
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            vec[j] = data1[i][j];
        }
        c_fft1d(vec, N, -1);
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            data1[i][j] = vec[j];
        }
    }

    free(vec);

    vec = (complex *)malloc(N * sizeof(complex));

    #pragma omp parallel for shared(data1,vec) private(i)
    for (i=lb;i<hb;i++) {
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            vec[j] = data2[i][j];
        }
        c_fft1d(vec, N, -1);
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            data2[i][j] = vec[j];
        }
    }

    free(vec);

    //Receving rows of data1, data2

    if(my_rank == 0){
        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Recv(data1[j], N, mystruct, i, tag, MPI_COMM_WORLD, &status);
                MPI_Recv(data2[j], N, mystruct, i, tag, MPI_COMM_WORLD, &status);
            }
        }
    }else{

        for(j = lb; j < hb; j++){
            MPI_Send(&data1[j][0], N, mystruct, 0, tag, MPI_COMM_WORLD);
            MPI_Send(&data2[j][0], N, mystruct, 0, tag, MPI_COMM_WORLD);
        }
    }

    //Starting and send columns of data1, data2

    if(my_rank == 0){
        transpose(data1, data3);
        transpose(data2, data4);

        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Send(&data3[j][0], N, mystruct, i, tag, MPI_COMM_WORLD);
                MPI_Send(&data4[j][0], N, mystruct, i, tag, MPI_COMM_WORLD);
            }
        }
    }else{
        for(j = lb; j < hb; j++){
            MPI_Recv(data3[j], N, mystruct, 0, tag, MPI_COMM_WORLD, &status);
            MPI_Recv(data4[j], N, mystruct, 0, tag, MPI_COMM_WORLD, &status);
        }

    }

    //Doing fft1d forward for data1 and data2 columns

    vec = (complex *)malloc(N * sizeof(complex));

    #pragma omp parallel for shared(data1,vec) private(i)
    for (i=lb;i<hb;i++) {
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            vec[j] = data3[i][j];
        }
        c_fft1d(vec, N, -1);
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            data3[i][j] = vec[j];
        }
    }

    free(vec);

    vec = (complex *)malloc(N * sizeof(complex));

    #pragma omp parallel for shared(data1,vec) private(i)
    for (i=lb;i<hb;i++) {
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            vec[j] = data4[i][j];
        }
        c_fft1d(vec, N, -1);
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            data4[i][j] = vec[j];
        }
    }

    free(vec);

    //Receving columns of data1, data2

    if(my_rank == 0){
        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Recv(data3[j], N, mystruct, i, tag, MPI_COMM_WORLD, &status);
                MPI_Recv(data4[j], N, mystruct, i, tag, MPI_COMM_WORLD, &status);
            }
        }
    }else{
        for(j = lb; j < hb; j++){
            MPI_Send(&data3[j][0], N, mystruct, 0, tag, MPI_COMM_WORLD);
            MPI_Send(&data4[j][0], N, mystruct, 0, tag, MPI_COMM_WORLD);
        }
    }


    if(my_rank == 0){
        transpose(data3,data1);
        transpose(data4,data2);
        mmpoint(data1, data2, data3);
    }

    //Starting and send rows of data1, data2

    if(my_rank == 0){
        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Send(&data3[j][0], N, mystruct, i, tag, MPI_COMM_WORLD);
            }
        }
    }else{

        for(j = lb; j < hb; j++){
            MPI_Recv(data3[j], N, mystruct, 0, tag, MPI_COMM_WORLD, &status);
        }

    }

    //Doing fft1d forward for data1 and data2 rows

    vec = (complex *)malloc(N * sizeof(complex));

    #pragma omp parallel for shared(data1,vec) private(i)
    for (i=lb;i<hb;i++) {
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            vec[j] = data3[i][j];
        }
        c_fft1d(vec, N, -1);
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            data3[i][j] = vec[j];
        }
    }

    free(vec);


    //Receving rows of data1, data2

    if(my_rank == 0){
        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Recv(data3[j], N, mystruct, i, tag, MPI_COMM_WORLD, &status);
            }
        }
    }else{
        for(j = lb; j < hb; j++){
            MPI_Send(&data3[j][0], N, mystruct, 0, tag, MPI_COMM_WORLD);
        }

    }

    //Starting and send columns of data1, data2

    if(my_rank == 0){
        transpose(data3,data4);

        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Send(&data4[j][0], N, mystruct, i, tag, MPI_COMM_WORLD);
            }
        }
    }else{
        for(j = lb; j < hb; j++){
            MPI_Recv(data4[j], N, mystruct, 0, tag, MPI_COMM_WORLD, &status);
        }

    }

    //Doing fft1d forward for data1 and data2 columns

    vec = (complex *)malloc(N * sizeof(complex));

    #pragma omp parallel for shared(data1,vec) private(i)
    for (i=lb;i<hb;i++) {
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            vec[j] = data4[i][j];
        }
        c_fft1d(vec, N, -1);
        #pragma omp parallel for shared(data1,vec) private(j)
        for (j=0;j<N;j++) {
            data4[i][j] = vec[j];
        }
    }

    free(vec);

    //Receving columns of data1, data2

    if(my_rank == 0){
        for(i=1;i<p;i++){
            offset=i*rows;
            for(j = offset; j < (offset+rows); j++){
                MPI_Recv(data4[j], N, mystruct, i, tag, MPI_COMM_WORLD, &status);
            }
        }
    }else{
        for(j = lb; j < hb; j++){
            MPI_Send(&data4[j][0], N, mystruct, 0, tag, MPI_COMM_WORLD);
        }

    }

    if(my_rank == 0){
        transpose(data4,data3);
        /* Stop Clock */
        stopTime = MPI_Wtime();

        printf("\nElapsed time = %lf s.\n",(stopTime - startTime));
        printf("--------------------------------------------\n");
    }

    MPI_Finalize();

    if(my_rank == 0){
        printfile(output_matrix, data3);
    }

    free(data1);
    free(data2);
    free(data3);
    free(data4);

    return 0;
}
