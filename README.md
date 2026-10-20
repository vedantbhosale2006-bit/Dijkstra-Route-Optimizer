# CCA-2: CI/CD Pipeline Implementation and Deployment of C++ Dijkstra Route Optimizer

A robust, automated Continuous Integration and Continuous Deployment (CI/CD) pipeline built on AWS infrastructure, containerizing and deploying a C++ route optimization system using Jenkins, Docker, Amazon ECR, and Minikube (Kubernetes).

## 👨‍💻 Student & Course Details

* **Course:** TYCSE Cloud Computing and DevOps
* **Student Name:** Vedant Rajesh Bhosale

## 🎥 Video Demonstration & Timestamps

* **Submission Video:** [Watch the Full CI/CD Demonstration on YouTube](https://www.youtube.com/watch?v=RJuKWBBxCgc)

### Chapter Timestamps

* **00:00** - Introduction & Project Overview
* **00:15** - AWS EC2 Infrastructure & Instance Setup
* **00:55** - Terminal Environment & Tool Verification (Java, Docker, kubectl, Minikube)
* **01:40** - GitHub Repository & Configuration Files
* **05:28** - Jenkins Pipeline Creation & Build Execution
* **07:00** - AWS ECR Console & Image Verification
* **07:12** - Minikube Deployment & Pod Status Verification
* **07:54** - Source Code Modification & Git Push (TESTING Menu Label)
* **08:27** - Pipeline Redeployment & Final Application Verification
* **09:27** - Conclusion & Closing Remarks

## 📁 Project Structure

```
.
├── .gitignore               # Git ignore rules
├── Dockerfile              # Docker container build specifications
├── Jenkinsfile             # Declarative 7-stage Jenkins CI/CD pipeline definition
├── MiniProjectPBLII.cpp    # Main C++ Dijkstra Route Optimizer source code
├── unamedNetwork0          # Pre-configured sample network dataset
└── k8s/
    ├── deployment.yaml     # Kubernetes Deployment manifest
    └── service.yaml        # Kubernetes Service manifest
```

> **Note on `unamedNetwork0`:** A preconfigured sample network graph dataset. Developers can pass this filename at runtime to instantly load the graph and display available operations/menu options without manual node entry.

## 🛠️ Infrastructure Prerequisites

* **Cloud Environment:** AWS EC2 Instance running Amazon Linux (utilizing `yum` package manager).
* **Container Registry:** Amazon ECR private repository (`cplusplus-router`) hosted in region `ap-south-1` (`118308531168.dkr.ecr.ap-south-1.amazonaws.com/cplusplus-router`).

## 💻 Complete Installation & Setup Guide (Amazon Linux / EC2)

Run the following commands on your Amazon Linux EC2 instance to install and configure all required prerequisites from scratch:

### 1. System Update & Dependencies

```bash
sudo yum update -y
sudo yum install -y curl unzip wget ca-certificates
```

### 2. Install Docker & Configure Permissions

```bash
sudo yum install -y docker
sudo systemctl start docker
sudo systemctl enable docker
sudo usermod -aG docker $USER
newgrp docker
```

### 3. Install AWS CLI v2

```bash
curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip"
unzip awscliv2.zip
sudo ./aws/install
aws --version
```

### 4. Install `kubectl` (Kubernetes CLI)

```bash
curl -LO "https://dl.k8s.io/release/$(curl -L -s https://dl.k8s.io/release/stable.txt)/bin/linux/amd64/kubectl"
sudo install -o root -g root -m 0755 kubectl /usr/local/bin/kubectl
kubectl version --client
```

### 5. Install Minikube

```bash
curl -LO https://storage.googleapis.com/minikube/releases/latest/minikube-linux-amd64
sudo install minikube-linux-amd64 /usr/local/bin/minikube
minikube start --driver=docker
```

### 6. Install Java & Jenkins (CI/CD Server)

```bash
sudo yum install -y java-17-openjdk
sudo wget -O /etc/yum.repos.d/jenkins.repo https://pkg.jenkins.io/redhat-stable/jenkins.repo
sudo rpm --import https://pkg.jenkins.io/redhat-stable/jenkins.io-2023.key
sudo yum install -y jenkins
sudo systemctl start jenkins
sudo systemctl enable jenkins
# Retrieve initial Jenkins administrator password:
sudo cat /var/lib/jenkins/secrets/initialAdminPassword
```

## ⚙️ Core Configuration Files

### 1. `Dockerfile`

```dockerfile
FROM debian:bookworm-slim
WORKDIR /app
COPY router /app/router
RUN chmod +x /app/router
CMD ["./router"]
```

### 2. `Jenkinsfile`

```groovy
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
                    def ecrRepoUri = "118308531168.dkr.ecr.ap-south-1.amazonaws.com/cplusplus-router"
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
                    sh 'kubectl apply --validate=false -f k8s/deployment.yaml'
                    sh 'kubectl apply --validate=false -f k8s/service.yaml'
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
```

### 3. `k8s/deployment.yaml`

```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: cplusplus-router-deployment
spec:
  replicas: 1
  selector:
    matchLabels:
      app: cplusplus-router
  template:
    metadata:
      labels:
        app: cplusplus-router
    spec:
      containers:
      - name: router
        image: 118308531168.dkr.ecr.ap-south-1.amazonaws.com/cplusplus-router:latest
        imagePullPolicy: IfNotPresent
```

### 4. `k8s/service.yaml`

```yaml
apiVersion: v1
kind: Service
metadata:
  name: cplusplus-router-service
spec:
  type: NodePort
  selector:
    app: cplusplus-router
  ports:
    - port: 80
      targetPort: 8080
```

## 🚀 Execution & Interactive Verification Commands

To compile locally, deploy to Minikube, or execute interactive debugging where you can input commands and provide graph inputs:

```bash
# Compile C++ source code with optimizations
g++ -O3 -o router MiniProjectPBLII.cpp

# Build Docker container image locally
docker build -t cplusplus-router:latest .

# Load image into Minikube and apply manifests
minikube image load cplusplus-router:latest
kubectl apply --validate=false -f k8s/deployment.yaml
kubectl apply --validate=false -f k8s/service.yaml

# Verify cluster resources
kubectl get pods
kubectl get deployment
kubectl get service

# Interactive Debugging / Running Container with Input Support:
kubectl exec -it deployment/cplusplus-router-deployment -- /app/router
```