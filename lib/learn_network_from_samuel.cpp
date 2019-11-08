#include<iostream>
#include<torch/torch.h>
#define params 21

using namespace std;

struct Model : torch::nn::Module {

    Model() {
        l1 = register_module("l1", torch::nn::Linear(params, 64));
        l2 = register_module("l2", torch::nn::Linear(64, 64));
        l3 = register_module("l3", torch::nn::Linear(64, 1));
        b1 = register_parameter("b1", torch::randn(64));
        b2 = register_parameter("b2", torch::randn(64));
    }

    torch::Tensor forward(torch::Tensor X) {
        X = torch::relu(l1->forward(X) + b1);
        X = torch::dropout(X, /*p=*/0.5, is_training());
        X = torch::relu(l2->forward(X) + b2);
        X = l3->forward(X);
        return X;
    }

    torch::nn::Linear l1{nullptr}, l2{nullptr}, l3{nullptr};
    torch::Tensor b1, b2;
};

class MyDataset : public torch::data::Dataset<MyDataset>
{
    private:
        torch::Tensor states_, labels_;

    public:
        explicit MyDataset(torch::Tensor& data, torch::Tensor& labels) 
            : states_(data), labels_(labels) {   };

        torch::data::Example<> get(size_t index) override {
            return {states_[index], labels_[index]};
        }

        torch::optional<size_t> size() const override {
            return labels_.size(0);
        };
};


int main()
{
    auto model = std::make_shared<Model>();
    // torch::load(model, "../best_model.pt");
    int lines; cin >> lines;

    torch::Tensor data = torch::zeros({lines, params}, torch::requires_grad(false));
    torch::Tensor labels = torch::zeros({lines, 1}, torch::requires_grad(false));

    double a;

    for(int i=0;i<lines;i++)
    {
        for(int j=0;j<params;j++) {cin >> a; data[i][j] = a;}
        cin >> a; labels[i] = a;
    }

    auto data_set = MyDataset(data, labels).map(torch::data::transforms::Stack<>());
    
    int64_t batch_size = 256;
    double lr = 1e-3;
    int64_t n_epochs = 20;
    int dataset_size = data_set.size().value();
    float best_mse = std::numeric_limits<float>::max();

    auto data_loader = torch::data::make_data_loader<torch::data::samplers::SequentialSampler>(
    std::move(data_set), batch_size);

    torch::optim::Adam optimizer(model->parameters(), torch::optim::AdamOptions(lr));

    for(int i=1;i<=n_epochs;i++)
    {
        int count = 0;
        float mse = 0;

        for (auto& batch : *data_loader)
        {
            auto states = batch.data;
            auto scores = batch.target;

            optimizer.zero_grad();
            auto output = model->forward(states);

            auto loss = torch::mse_loss(output, scores);

            loss.backward();
            optimizer.step();

            mse += loss.template item<float>();
            count++;
        }

        mse /= count;
        cout << "Mean squared error: " << mse << "\n";   

        if(mse < best_mse)
        {
            torch::save(model, "../best_model.pt");
            best_mse = mse;
        }
    }
}
