pipeline {
    agent any

    stages {
        stage('1. Source Code') {
            steps {
                script {
                    echo 'Verifying source code availability in workspace...'
                    sh 'ls -la MiniProjectPBLII.cpp'
                }
            }
        }

        stage('2. Jenkins Checkout') {
            steps {
                checkout scm
            }
        }

        stage('3. Build and Test') {
            steps {
                script {
                    sh 'g++ -O3 -o router MiniProjectPBLII.cpp'
                    echo 'Build successful. (Note: Using build check in lieu of automated tests per course tip).'
                }
            }
        }

        stage('4. Docker Build') {
            steps {
                script {
                    sh 'docker build -t cplusplus-router:latest .'
                }
            }
        }

        stage('5. Push Image') {
            steps {
                script {
                    def awsRegion = "ap-south-1"
                    def ecrRepoUri = "118308531168.dkr.ecr.ap-south-1.amazonaws.com/cplusplus-router" // Replace with your actual ECR URI
                    sh "aws ecr get-login-password --region ${awsRegion} | docker login --username AWS --password-stdin ${ecrRepoUri.split('/')[0]}"
                    sh "docker tag cplusplus-router:latest ${ecrRepoUri}:latest"
                    sh "docker push ${ecrRepoUri}:latest"
                }
            }
        }

        stage('6. Minikube Deployment') {
            steps {
                script {
                    sh 'minikube image load cplusplus-router:latest || true'
                    sh 'kubectl apply -f k8s/deployment.yaml'
                    sh 'kubectl apply -f k8s/service.yaml'
                }
            }
        }

        stage('7. Verification') {
            steps {
                script {
                    sh 'kubectl get pods'
                    sh 'kubectl get deployment'
                    sh 'kubectl get service'
                }
            }
        }
    }

    post {
        success {
            echo 'Pipeline completed successfully through all 7 stages!'
        }
        failure {
            echo 'Pipeline failed during execution. Check stage logs for details.'
        }
    }
}