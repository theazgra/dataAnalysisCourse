
### Top 20 breeds - choosen from test dataset
- Those nets are overkill for 20 breeds but we can compare them with our own architecture
- First values are training results and then testing result:
- VGG19                 - loss: 1.0870 - accuracy: 0.7289 - val_loss: 1.7230 - val_accuracy: 0.4750
                        - loss: 1.6196 - accuracy: 0.5040
  - augmentation-basic
        - 20 epochs     - loss: 1.8372 - accuracy: 0.4551 - val_loss: 1.7726 - val_accuracy: 0.4300
                        - loss: 1.7192 - accuracy: 0.4808
        - 40 epochs
                        - loss: 1.5480 - accuracy: 0.5418 - val_loss: 1.5707 - val_accuracy: 0.5150
                        - loss: 1.5665 - accuracy: 0.5102
  - augmentation-wshift
        - 40 epochs
                        - loss: 1.5957 - accuracy: 0.5276 - val_loss: 1.6098 - val_accuracy: 0.4800
                        - loss: 1.5811 - accuracy: 0.5036

- InceptionV3           - loss: 0.0012 - accuracy: 1.0000 - val_loss: 0.0807 - val_accuracy: 0.9650
                        - loss: 0.1768 - accuracy: 0.9599

- Xception              - loss: 0.0023 - accuracy: 1.0000 - val_loss: 0.1054 - val_accuracy: 0.9650
                        - loss: 0.1250 - accuracy: 0.9626

- InceptionResnet       - loss: 0.0047 - accuracy: 0.9994 - val_loss: 0.0758 - val_accuracy: 0.9800
                        - loss: 0.1677 - accuracy: 0.9603


- On best one we try some shenanigans
```py
model = keras.Sequential(layers=[
  keras.layers.Input(shape=(IMG_DIM, IMG_DIM, 3), dtype=np.float32),
  keras.layers.Conv2D(input_shape=(IMG_DIM, IMG_DIM, 3),filters=64,kernel_size=(5,5),activation='relu',padding='same',),
  keras.layers.Conv2D(filters=64,kernel_size=(5,5),activation='relu',padding='same'),
  keras.layers.Dropout(rate=0.15),
  keras.layers.Conv2D(filters=64,kernel_size=(5,5),activation='relu',padding='same'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=64,kernel_size=(5,5),activation='relu',padding='same'),
  keras.layers.Conv2D(filters=64,kernel_size=(5,5),activation='relu',padding='same'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=128,kernel_size=(5,5),activation='relu',padding='same'),
  keras.layers.Dropout(rate=0.15),
  keras.layers.Conv2D(filters=128,kernel_size=(5,5),activation='relu',padding='same'),
  keras.layers.Conv2D(filters=128,kernel_size=(5,5),activation='relu',padding='same'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=256,kernel_size=(3,3),activation='relu',padding='same'),
  keras.layers.Conv2D(filters=256,kernel_size=(3,3),activation='relu',padding='same'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=256,kernel_size=(3,3),activation='relu',padding='same'),
  keras.layers.Conv2D(filters=256,kernel_size=(3,3),activation='relu',padding='same'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.GlobalMaxPooling2D(),
  keras.layers.Dense(units = CLASS_COUNT, activation='softmax')
])
```
  - loss: 2.9957 - accuracy: 0.0522 - val_loss: 3.0019 - val_accuracy: 0.0300
  - val_accuracy is stuck cann't get better numbers, whole model is stuck
  - same story withotu Dropouts
  - same story with augmentation
```py
model = keras.Sequential(layers=[
  keras.layers.Input(shape=(IMG_DIM, IMG_DIM, 3), dtype=np.float32),
  keras.layers.Conv2D(filters=128,kernel_size=(7,7),activation='relu',padding='same',),
  keras.layers.Conv2D(filters=128,kernel_size=(7,7),activation='relu'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=128,kernel_size=(5,5),activation='relu'),
  keras.layers.Conv2D(filters=128,kernel_size=(5,5),activation='relu'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=256,kernel_size=(5,5),activation='relu'),
  keras.layers.Conv2D(filters=256,kernel_size=(5,5),activation='relu',),
  keras.layers.MaxPooling2D(pool_size=(4,4)),
  keras.layers.Conv2D(filters=256,kernel_size=(3,3),activation='relu'),
  keras.layers.Conv2D(filters=256,kernel_size=(3,3),activation='relu'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.GlobalMaxPooling2D(),
  keras.layers.Dense(units = CLASS_COUNT, activation='softmax')
])
```
  - No improvements
```py
model = keras.Sequential(layers=[
  keras.layers.Input(shape=(IMG_DIM, IMG_DIM, 3), dtype=np.float32),
  keras.layers.Conv2D(filters=128,kernel_size=(7,7),activation='relu',padding='same',),
  keras.layers.Conv2D(filters=128,kernel_size=(7,7),activation='relu'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=128,kernel_size=(5,5),activation='relu'),
  keras.layers.Conv2D(filters=128,kernel_size=(5,5),activation='relu'),
  keras.layers.MaxPooling2D(pool_size=(2,2)),
  keras.layers.Conv2D(filters=256,kernel_size=(5,5),activation='relu'),
  keras.layers.Conv2D(filters=256,kernel_size=(5,5),activation='relu',),
  keras.layers.MaxPooling2D(pool_size=(4,4)),
  keras.layers.Conv2D(filters=256,kernel_size=(3,3),activation='relu'),
  keras.layers.Flatten(),
  keras.layers.Dense(64, activation='relu'),
  keras.layers.Dense(CLASS_COUNT)
])
```
  - No improvements
```py
model = keras.Sequential(layers=[
  keras.layers.Input(shape=(IMG_DIM, IMG_DIM, 3), dtype=np.float32),
  keras.layers.Conv2D(32, (9,9), activation='relu'),
  keras.layers.Conv2D(32, (9,9), activation='relu'),
  keras.layers.MaxPooling2D((2, 2)),
  keras.layers.Conv2D(64, (7,7), activation='relu'),
  keras.layers.Conv2D(64, (7,7), activation='relu'),
  keras.layers.MaxPooling2D((2, 2)),
  keras.layers.Conv2D(128, (5,5), activation='relu'),
  keras.layers.Conv2D(128, (5,5), activation='relu'),
  keras.layers.MaxPooling2D((2, 2)),
  keras.layers.Conv2D(128, (3,3), activation='relu'),
  keras.layers.Conv2D(128, (3,3), activation='relu'),
  keras.layers.MaxPooling2D((2, 2)),
  keras.layers.GlobalMaxPooling2D(),
  keras.layers.Dense(class_count, activation='softmax')
])
```
```txt
Epoch 1/20
57/57 [==============================] - 14s 242ms/step - loss: 3.5686 - accuracy: 0.0483 - val_loss: 3.0665 - val_accuracy: 0.0400
Epoch 2/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0582 - accuracy: 0.0456 - val_loss: 3.0238 - val_accuracy: 0.0500
Epoch 3/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0432 - accuracy: 0.0406 - val_loss: 3.0201 - val_accuracy: 0.0600
Epoch 4/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0331 - accuracy: 0.0422 - val_loss: 3.0189 - val_accuracy: 0.0350
Epoch 5/20
57/57 [==============================] - 13s 229ms/step - loss: 3.0303 - accuracy: 0.0461 - val_loss: 3.0244 - val_accuracy: 0.0500
Epoch 6/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0357 - accuracy: 0.0450 - val_loss: 3.0114 - val_accuracy: 0.0400
Epoch 7/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0202 - accuracy: 0.0450 - val_loss: 3.0248 - val_accuracy: 0.0450
Epoch 8/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0294 - accuracy: 0.0422 - val_loss: 3.0208 - val_accuracy: 0.0500
Epoch 9/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0209 - accuracy: 0.0511 - val_loss: 3.0137 - val_accuracy: 0.0350
Epoch 10/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0166 - accuracy: 0.0494 - val_loss: 3.0031 - val_accuracy: 0.0650
Epoch 11/20
57/57 [==============================] - 13s 229ms/step - loss: 3.0165 - accuracy: 0.0461 - val_loss: 3.0037 - val_accuracy: 0.0400
Epoch 12/20
57/57 [==============================] - 13s 229ms/step - loss: 3.0130 - accuracy: 0.0511 - val_loss: 2.9979 - val_accuracy: 0.0450
Epoch 13/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0146 - accuracy: 0.0422 - val_loss: 3.0149 - val_accuracy: 0.0550
Epoch 14/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0152 - accuracy: 0.0472 - val_loss: 3.0081 - val_accuracy: 0.0350
Epoch 15/20
57/57 [==============================] - 13s 229ms/step - loss: 3.0123 - accuracy: 0.0500 - val_loss: 3.0013 - val_accuracy: 0.0650
Epoch 16/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0120 - accuracy: 0.0450 - val_loss: 3.0083 - val_accuracy: 0.0350
Epoch 17/20
57/57 [==============================] - 13s 227ms/step - loss: 3.0113 - accuracy: 0.0561 - val_loss: 3.0174 - val_accuracy: 0.0400
Epoch 18/20
57/57 [==============================] - 13s 227ms/step - loss: 3.0129 - accuracy: 0.0483 - val_loss: 3.0097 - val_accuracy: 0.0650
Epoch 19/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0129 - accuracy: 0.0361 - val_loss: 3.0063 - val_accuracy: 0.0400
Epoch 20/20
57/57 [==============================] - 13s 228ms/step - loss: 3.0105 - accuracy: 0.0428 - val_loss: 3.0116 - val_accuracy: 0.0350
```


### Inception - t60
- NoGenerator - loss: 0.5348 - acc: 0.8914
- Augmented   - loss: 0.3541 - acc: 0.8988

## All breeds results

### Inception
- Base - NoDenseLayers          - loss: 1.2912 - acc: 0.8501
- Dense512                      - loss: 0.7459 - acc: 0.8367
- 2xDense512                    - loss: 0.7989 - acc: 0.8338
- 2xDense512+Dense256           - loss: 0.7278 - acc: 0.8325
- 2xDense512-Augmented          - loss: 0.6262 - acc: 0.8169
- 2xDense512-Augmented - Scaled - loss: 0.4369 - acc: 0.8676 <---
### Xception
- Base                          - loss: 0.8652 - acc: 0.8469
### Inception-ResNet
- Base                          - loss: 0.9943 - acc: 0.8693 <-- best and without scaling !!!
### Own net scaled down
- Very bad results