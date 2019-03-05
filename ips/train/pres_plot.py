
import numpy as np
import matplotlib.pyplot as plt
import scipy.stats as stats


def f1_plot(results,title=None,save=None,
	req=0.95,level=0.95,metric='val_f1',
	color='black',figsize=(12,8)):
	"""
	f1_plot() generates a plot for average f1 score over the course of training 
	a neural network several times. The plot includes raw data as well as average 
	curve and error bars

	Inputs:
	results: dictionary of metrics resulting from repeated trainings of a network
	title: a string to be included as a plot title
	save: a name for the plot to be saved to a png file with
	req: Average F1 score requirement to be displayed on the plot
	level: Statistical confidence level to calculate confidence intervals with
	metric: change this to plot other keys of the results input
	"""
	# Color themes
	if color == 'black':
		text_color='white'
		face_color='xkcd:charcoal'
		line_color='xkcd:cyan'
	elif color=='white':
		text_color='black'
		face_color='white'
		line_color='xkcd:teal'

	M, epochs = np.array(results[metric]).shape
	mean_f1 = np.mean(results[metric],axis=0)

	# Amount of uncertainty based on confidence level
	std_f1 = np.std(results[metric],axis=0)
	unc_f1 = stats.t.ppf(1/2+level/2,df=M-1) * std_f1/np.sqrt(M)
	eax = np.arange(epochs)+1


	fig,ax = plt.subplots(1,1,figsize=figsize,facecolor=color)

	if title:
		plt.title(title,fontsize=24,color=text_color)

	plt.xlabel('Rounds of Training (Epochs)',fontsize=18)
	plt.ylabel('F1 Score',fontsize=18)
	ax.xaxis.label.set_color(text_color)
	ax.yaxis.label.set_color(text_color)
	ax.tick_params(colors=text_color)
	ax.set_facecolor(face_color)
	meas = plt.plot(eax,np.transpose(results[metric]),'o',
	  markersize=8,
	  alpha=3/M,
	  color=text_color)
	# Line for the mean
	# plt.plot(eax,mean_f1,
	#   linewidth=2,
	#   color='w',
	#   label='Mean F1 Score')
	plt.errorbar(eax,mean_f1,
		yerr=unc_f1,
		fmt='.-',
		linewidth=2,
		elinewidth=4,
		capsize=4,
		color=text_color,
		ecolor=line_color,
		label=f'Mean F1 Score with {100*level:.0f}% Confidence Intervals')
	plt.axhline(y=req,
	  linestyle='--',
	  color='xkcd:pale green',
	  linewidth=2,
	  label='F1 Score Requirement')
	if metric.find('f1')>1 or metric.find('acc')>1 :
		plt.axis((0,epochs+1,0.8,1))
	ax.set_xticks(np.arange(0,epochs+1,2))
	xbox = [0,epochs+1,epochs+1,0]
	ybox = [1,1,req,req]
	plt.fill(xbox,ybox,
		alpha = 0.15,
		color='xkcd:light green',
		label=f'Region for Requirement F1 >= {req:.2f}')
	plt.grid(True,color='grey')
	leg = plt.legend(facecolor='grey',fontsize=14)
	plt.setp(leg.get_texts(),color=text_color)
	# save the fig is we want to
	if save:
		plt.savefig(save+'.png',
			facecolor=face_color,edgecolor=color)

	plt.show()


# plt.plot(np.arange(epochs),mean_f1+unc_f1,'-',
#   color='xkcd:green',
#   linewidth=2,
#   label=f'{100*level:.0f}% Confidence Interval Upper Limit')
# plt.plot(np.arange(epochs),mean_f1-unc_f1,'-',
#   color='xkcd:red',
#   linewidth=2,
#   label=f'{100*level:.0f}% Confidence Interval Lower Limit')

# plt.xlim([0,epochs-1])
# # plt.ylim([0,1])
# plt.grid(True)
# plt.xlabel('Training Epochs',
#   fontsize=16)
# plt.ylabel('Mean F1 Score',
#   fontsize=16)
# plt.title(
#   f'Mean F1 Scores at Each Epoch\n' \
#   f'Feature Detector:{args["model"]}, ' \
#   f'Trained on {N_im} Images{(", Resampled" if args["resampling"] else "")}',
#   fontsize=20)
# h,l = plt.gca().get_legend_handles_labels()
# h1 = [meas[0]]+h
# l1 = ['F1 Score Measuremets']+l
# plt.legend(h1,l1,fontsize=12,loc='best')
# # Save this figure to a png also
# savefile = f'{plotdir}mean_f1_{args["model"]}_e{epochs}_n{N_im}_m{M}.png'
# i=1
# while os.path.isfile(savefile):
#   savefile = f'{plotdir}mean_f1_{args["model"]}_e{epochs}_n{N_im}_m{M}_{i}.png'
#   i+=1

# plt.savefig(savefile)
# print(f'[INFO] Mean F1 chart saved to {savefile}')
# plt.show()

def plot_errors(classifier,ground_truth,test_inputs,modelname=None,mono=False):

  predictions = classifier.predict(test_featrs).reshape((nTest,))
  #find where the errors in the testing data are
  errors = np.where((predictions>0.5) != ground_truth)[0]
  print("Number of Errors = {}/{}".format(len(errors),nTest))
  # Create this list thing for plotting examples
  from itertools import chain
  images = chain(errors,range(10))

  for i in images:
    print(f'Image source: {test_list[i]}')
    # Print the classification and prediction for this case
    print(f'Image does {"" if ground_truth[i] else "not"} contain aurora,'
          f' Probability: {predictions[i]}')
    # Show the image for reference
    plt.imshow(test_inputs[i,:,:,0],cmap='inferno')
    plt.title(f'Predicted Aurora Probability = {100*predictions[i]:.2f}%')
    plt.grid(False)
    plt.show()


def plot_history(history,title=None,save=None,
				figsize=(12,8),loss=False,
				color='black',req=0.95):
	"""
	f1_plot() generates a plot for average f1 score over the course of training 
	a neural network several times. The plot includes raw data as well as average 
	curve and error bars

	Inputs:
	results: dictionary of metrics resulting from repeated trainings of a network
	title: a string to be included as a plot title
	save: a name for the plot to be saved to a png file with
	req: Average F1 score requirement to be displayed on the plot
	level: Statistical confidence level to calculate confidence intervals with
	metric: change this to plot other keys of the results input
	"""
	# Colorthemes
	if color == 'black':
		text_color='white'
		face_color='xkcd:charcoal'
	elif color=='white':
		text_color='black'
		face_color='white'

	epochs = len(history['acc'])
	eax = np.arange(epochs)+1

	fig,ax = plt.subplots(1,1,figsize=figsize,facecolor=color)
	if loss:
		plt.axis((0,epochs,0,1))
	else:
		plt.axis((0,epochs,0.8,1))

	if title:
		plt.title(title,fontsize=24,color=text_color)

	if loss:
		plt.plot(eax, history['loss'],
			linewidth=2,
			label='Loss on Training Data')
		plt.plot(eax, history['val_loss'],
			linewidth=2,
			label='Loss on Validation Data')
	else:
		plt.plot(eax, history['acc'],
			linewidth=2,
			label='Accuracy on Training Data')
		plt.plot(eax, history['f1'],
			linewidth=2,
			label='F1 Score on Training Data')
		plt.plot(eax, history['val_acc'],
			linewidth=2,
			label='Accuracy on Validation Data')
		plt.plot(eax, history['val_f1'],
			linewidth=2,
			label='F1 Score on Validation Data')
		xbox = [0,epochs+1,epochs+1,0]
		ybox = [1,1,req,req]
		plt.fill(xbox,ybox,
			alpha = 0.15,
			color='xkcd:light green',
			label=f'Region for Requirement F1 >= {req:.2f}')
		plt.axhline(y=req,
		  linestyle='--',
		  color='xkcd:pale green',
		  linewidth=2,
		  label='F1 Score Requirement')


	plt.xlabel('Rounds of Training (Epochs)',fontsize=18)
	plt.ylabel('Metric Score',fontsize=18)
	ax.xaxis.label.set_color(text_color)
	ax.yaxis.label.set_color(text_color)
	ax.tick_params(colors=text_color)
	ax.set_facecolor(face_color)
	# if metric.find('f1')>1 or metric.find('acc')>1 :
	# plt.axis((0,epochs+1,0.8,1))
	# ax.set_xticks(np.arange(0,epochs+1,2))
	
	plt.grid(True,color='grey')
	leg = plt.legend(facecolor='grey',fontsize=14)
	plt.setp(leg.get_texts(),color=text_color)
	# save the fig is we want to
	if save:
		plt.savefig(save+'.png',
			facecolor=face_color,edgecolor=color)

	plt.show()